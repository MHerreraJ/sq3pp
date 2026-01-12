#include <sq3pp/Database.h>
#include <sq3pp/Statement.h>
#include <sq3pp/Transaction.h>
#include <sq3pp/Exception.h>
#include <stdexcept>

using namespace sq3pp;
Database::Database() : _handle(nullptr) {}

Database::Database(const char* dbName) : _handle(nullptr) {
    open(dbName);
}

Database::Database(const std::string& dbName) : _handle(nullptr) {
    open(dbName);
}

Database::Database(Database&& other) noexcept : _handle(std::move(other._handle)) {}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        close();
        _handle = std::move(other._handle);
    }
    return *this;
}

Database::~Database() {
    close();
}


int Database::open(const char* dbName) {
    int rc = SQLITE_OK;
    sqlite3* handle = nullptr;
    if (_handle) {
        close();
    }
    if(dbName == nullptr) {
        return SQLITE_MISUSE;
    }
    rc = sqlite3_open(dbName, &handle);
    if (rc == SQLITE_OK) {
        _handle = std::shared_ptr<sqlite3>(handle, sqlite3_close);
    } else if (handle) {
        // SQLite may return a handle even on failure - must close it
        sqlite3_close(handle);
    }
    return rc;
}

int Database::open(const std::string& dbName) {
    return open(dbName.c_str());
}

void Database::close() {
    if (isOpen()) {
        _handle.reset();
    }
}

struct ExecuteCallback{
    std::function<int(void*, int, char**, char**)> userCallback;
    void* userData;
};

static int execCallback(void* data, int argc, char** argv, char** azColName) {
    ExecuteCallback* execCb = reinterpret_cast<ExecuteCallback*>(data);
    if(execCb && execCb->userCallback){
        return execCb->userCallback(execCb->userData, argc, argv, azColName);
    }
    return 0;
}

int Database::execute(const std::string& sql, std::function<int(void*,int, char**, char**)> callback, void* data) {
    if(!isOpen()){
        return SQLITE_MISUSE;
    }
    char* errMsg = nullptr;
    ExecuteCallback execCb{callback, data};

    int rc = sqlite3_exec(_handle.get(), sql.c_str(), execCallback, &execCb, &errMsg);
    if (rc != SQLITE_OK) {
        if (errMsg) {
            sqlite3_free(errMsg);
        }
    }
    return rc;
}

Statement Database::createStatement(const std::string& query) {
    if(!isOpen()){
        throw DatabaseException(SQ3::ERROR, "Cannot create statement: database is not open.");
    }
    return Statement(_handle, query);
}

Transaction Database::beginTransaction() {
    if(!isOpen()){
        throw DatabaseException(SQ3::ERROR, "Cannot begin transaction: database is not open.");
    }
    return Transaction(_handle);
}