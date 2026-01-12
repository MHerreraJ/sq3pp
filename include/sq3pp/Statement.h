#ifndef SQ3PP_STATEMENT_H
#define SQ3PP_STATEMENT_H

#include <cstdint>
#include <memory>
#include <iostream>
#include <functional>
#include <sq3pp/Exception.h>
#include <sq3pp/Database.h>

namespace sq3pp{
class CellValue{
    public:
    enum class Type{
        INTEGER,
        DOUBLE,
        TEXT,
        BLOB,
        NULLTYPE
    };

    public:
    CellValue();
    CellValue(int iValue);
    CellValue(double dValue);
    CellValue(const std::string& strValue);
    CellValue(const char* cstrValue);
    CellValue(std::nullptr_t);
    CellValue(void* blob_value, int n);

    CellValue(const CellValue& other);
    CellValue( CellValue&& other) noexcept;
    CellValue& operator=(const CellValue& other);
    CellValue& operator=( CellValue&& other) noexcept;
    
    virtual ~CellValue();
    
    Type valueType() const {return _type;}
    bool isNull() const {return _type == Type::NULLTYPE;}
    bool isInteger() const {return _type == Type::INTEGER;}
    bool isDouble() const {return _type == Type::DOUBLE;}
    bool isText() const {return _type == Type::TEXT;}
    bool isBlob() const {return _type == Type::BLOB;}

    template<typename T>
    T valueAs() const;
    
    explicit operator bool() const { return !isNull(); }

    protected:
    Type _type;
    union{
        int _iValue;
        double _dValue;
        char* _strValue;
        struct{
            unsigned char* data;
            int size;
        } _blobValue;
    };
};


class Row{
    private:
    Row(int rowIndex, std::shared_ptr<sqlite3_stmt> stmt);
    Row(Row&& other);
    Row& operator=(Row&& other);
    
    public:
    Row() = delete;
    Row(const Row& other) = delete;
    Row& operator=(const Row& other) = delete;
    
    class Cell{
        private:
        Cell(int columnIndex, Row* row);
        
        public:
        Cell(const Cell& other) = delete;
        Cell& operator=(const Cell& other) = delete;
        Cell(Cell&& other);
        Cell& operator=(Cell&& other);
        

        bool isNull() const;
        explicit operator bool() const{ return !isNull(); }

        template<typename T>
        T valueAs(bool autoConvert=true) const;


        int valueType() const {return _type;}
        const char* columnName() const;

        private:
        int _column;
        int _type;
        Row* _parent;
        friend class Row;
    };

    class iterator{
        private:
        iterator(int index, Row& row);

        public:
        iterator(const iterator& other) = delete;
        iterator(iterator&& other) = delete;
        iterator& operator=(const iterator& other) = delete;
        iterator& operator=(iterator&& other) = delete;

        iterator& operator++();
        bool operator!=(const iterator& other);
        const Cell& operator*() const ;
        const Cell* operator->() const {return &_cell;}

        private:
        int _index;
        Row& _row;
        Cell _cell;
        friend class Row;
    };


    iterator begin();
    iterator end();
    int getRowIndex() const {return _rowIndex;}
    int getColumnCount() const {return _columnCount;}

    CellValue valueAt(int columnIndex);    
    CellValue operator[](int columnIndex);
    CellValue operator[](const std::string& colName);

    private:
    int _rowIndex;
    int _columnCount;
    std::shared_ptr<sqlite3_stmt> _stmt;
    friend class Statement;
};



class Statement{
    private:
    class Binder{
        private:
        Binder(Statement& parent, int index): parentStmt(parent), _index(index), _id("") {}
        Binder(Statement& parent, const std::string& id): parentStmt(parent), _index(-1), _id(id) {}

        Statement& parentStmt;
        int _index;
        std::string _id;

        public:
        Binder& operator=(const Binder& other) = delete;
        Binder& operator=(Binder&& other) = delete;
        Binder& operator=(int value);
        Binder& operator=(double value);
        Binder& operator=(const std::string& value);
        Binder& operator=(const char* value);
        Binder& operator=(std::nullptr_t);
        Binder& operator=(const std::vector<uint8_t>& blob_value);
        friend class Statement;
    };

    Statement(std::shared_ptr<sqlite3> handle, const std::string& query);

    public:
    Statement();
    Statement(const Statement& other) = delete;
    Statement(Statement&& other) noexcept;
    Statement& operator=(const Statement& other) = delete;
    Statement& operator=(Statement&& other) noexcept;
    virtual ~Statement();

    
    bool isValid() const {return _stmt != nullptr;}
    void reset(bool clearBindings = true);
    void finalize();
    

    // Bind methods with optional index parameter
    // If index is <0, use the next available index (internal counter) and increment it
    // Otherwise, bind to the specified index (0-based in the API, converted to 1-based for SQLite internally)
    // This allows both sequential and random access binding    
    Statement& bind(const std::string& value, int index = -1);
    Statement& bind(const char* value, int index = -1);
    Statement& bind(int value, int index = -1);
    Statement& bind(double value, int index = -1);
    Statement& bind(std::nullptr_t, int index = -1);
    Statement& bind(void* blob_value, int n, int index = -1);
    Statement& bind(const std::vector<uint8_t>& blob_value, int index = -1);
    Statement& bind(const CellValue& cellValue, int index = -1);


    // Bind by parameter name
    // The index is determined by looking up the parameter name in the prepared statement
    // Parameter names should not include the leading ':' or '@' or '$' used in SQLite
    // If the index is found, bind the value to that index and update the internal counter accordingly
    // If the parameter name is not found, return an error code
    Statement& bindById(const std::string& id, const std::string& value);
    Statement& bindById(const std::string& id, const char* value);
    Statement& bindById(const std::string& id, std::nullptr_t);
    Statement& bindById(const std::string& id, int value);
    Statement& bindById(const std::string& id, double value);
    Statement& bindById(const std::string& id, void* blob_value, int n);
    Statement& bindById(const std::string& id, const std::vector<uint8_t>& blob_value);
    Statement& bindById(const std::string& id, const CellValue& cellValue);


    Binder operator[](int index){return Binder(*this, index);}
    Binder operator[](const std::string& id){return Binder(*this, id);}


    // Get the index for a parameter name (0-based)
    int getIndexForId(const std::string& id);

    // Reset the internal bind index counter (0-based)
    void resetBindIndex(int index=0);


    SQ3 step(std::function<void(Row& row)> onRowFound = nullptr);


    // Return the number of rows affected or retrieved by the execution
    int execute();
    int execute(std::function<void(Row& row)> onRowFound);
    int execute(std::vector<std::vector<CellValue>>& outRows, std::vector<std::string>* outColumnNames = nullptr);

    Row& getCurrentRow(){ return _currentRow;}

    
    private:
    std::shared_ptr<sqlite3_stmt> _stmt;
    std::string _query;
    int _bindIndex;
    int _rowIndex;
    Row _currentRow;
    friend class Database;
};

}

std::ostream& operator<<(std::ostream& os, const sq3pp::CellValue& cellValue);
std::ostream& operator<<(std::ostream& os, const sq3pp::Row::Cell& cell);

#endif // SQ3PP_STATEMENT_H