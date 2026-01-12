#ifndef SQ3PP_TRANSACTION_H
#define SQ3PP_TRANSACTION_H

#include <sq3pp/Database.h>
namespace sq3pp{
class Transaction{
    private:
        Transaction(std::shared_ptr<sqlite3> database);
    public:
        ~Transaction();

        void commit();
        void rollback();

    private:
        std::shared_ptr<sqlite3> _dbHandle;
        bool _committed;
        friend class Database;
};
}
#endif // SQ3PP_TRANSACTION_H