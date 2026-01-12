#include <stdexcept>
#include <sq3pp/Exception.h>
#include <sq3pp/Transaction.h>

using namespace sq3pp;

Transaction::Transaction(std::shared_ptr<sqlite3> database) 
    : _dbHandle(database), _committed(false) {
    if(!_dbHandle) {
        throw DatabaseException(SQ3::NOT_OPEN, "Cannot create transaction: database is not open.");
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_dbHandle.get(), "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if(rc != SQLITE_OK) {
        std::string strMsg = errMsg ? std::string(errMsg) : "Unknown error";
        sqlite3_free(errMsg);
        throw DatabaseException(static_cast<SQ3>(rc), "Failed to begin transaction: " + strMsg);
    }
}

Transaction::~Transaction() {
    if(!_committed) {
        try{
            rollback();
        } catch(...) {
            // Suppress all exceptions in destructor
        }
    }
}

void Transaction::commit() {
    if(_committed) {
        throw DatabaseException(SQ3::MISUSE, "Transaction has already been finalized.");
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_dbHandle.get(), "COMMIT;", nullptr, nullptr, &errMsg);
    if(rc != SQLITE_OK) {
        std::string strMsg = errMsg ? std::string(errMsg) : "Unknown error";
        sqlite3_free(errMsg);
        throw DatabaseException(static_cast<SQ3>(rc), "Failed to commit transaction: " + strMsg);
    }
    _committed = true;
}

void Transaction::rollback() {
    if(_committed) {
        throw DatabaseException(SQ3::MISUSE, "Transaction has already been finalized.");
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_dbHandle.get(), "ROLLBACK;", nullptr, nullptr, &errMsg);
    if(rc != SQLITE_OK) {
        std::string strMsg = errMsg ? std::string(errMsg) : "Unknown error";
        sqlite3_free(errMsg);
        throw DatabaseException(static_cast<SQ3>(rc), "Failed to rollback transaction: " + strMsg);
    }
    _committed = true;
}
