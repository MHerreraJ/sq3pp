#ifndef SQ3PP_EXCEPTION_H
#define SQ3PP_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace sq3pp{

enum class SQ3{
    OK = 0,
    ERROR = 1,
    INTERNAL = 2,
    PERM = 3,
    ABORT = 4,
    BUSY = 5,
    LOCKED = 6,
    NOMEM = 7,
    READONLY = 8,
    INTERRUPT = 9,
    IOERR = 10,
    CORRUPT = 11,
    NOTFOUND = 12,
    FULL = 13,
    CANTOPEN = 14,
    PROTOCOL = 15,
    EMPTY = 16,
    SCHEMA = 17,
    TOOBIG = 18,
    CONSTRAINT = 19,
    MISMATCH = 20,
    MISUSE = 21,
    NOLFS = 22,
    AUTH = 23,
    FORMAT = 24,
    RANGE = 25,
    NOTADB = 26,
    NOTICE = 27,
    WARNING = 28,
    ROW = 100,
    DONE = 101,

    NOT_OPEN=2000,
};

class DatabaseException : public std::runtime_error{
    public:
    DatabaseException(SQ3 code, const std::string& message)
        : std::runtime_error(message), _code(code), _message(message) {}
    
    virtual ~DatabaseException() noexcept {}

    SQ3 code() const {
        return _code;
    }

    private:
    SQ3 _code;
    std::string _message;
};
}

#endif // SQ3PP_EXCEPTION_H