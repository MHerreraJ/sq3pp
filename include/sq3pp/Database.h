#ifndef SQ3PP_DATABASE_H
#define SQ3PP_DATABASE_H

#include <string>
#include <memory>
#include <functional>
#include <sqlite3.h>

namespace sq3pp{

class Statement;
class Transaction;

class Database{
public:
    Database();
    Database(const char* dbName);
    Database(const std::string& dbName);
    Database(const Database& other) = delete;
    Database(Database&& other) noexcept;
    Database& operator=(const Database& other) = delete;
    Database& operator=(Database&& other) noexcept;


    virtual ~Database();

    int open(const char* dbName);
    int open(const std::string& dbName);

    
    inline bool isOpen() const {
        return _handle != nullptr;
    }
    
    void close();

    int execute(const std::string& sql, std::function<int(void*, int, char**, char**)> callback = nullptr, void* data = nullptr);

    //operator for if (db)
    inline explicit operator bool() const {
        return isOpen();
    }

    sqlite3* getHandle() const {
        return _handle.get();
    }

    Statement createStatement(const std::string& query);
    Transaction beginTransaction();

private:
    std::shared_ptr<sqlite3> _handle;
};

}
#endif // SQ3PP_DATABASE_H