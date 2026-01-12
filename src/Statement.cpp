#include <sq3pp/Statement.h>
#include <sq3pp/Exception.h>
#include <cstring>
using namespace sq3pp;


template<>
CellValue Row::Cell::valueAs<CellValue>(bool autoConvert) const;

CellValue::CellValue() : _type(Type::NULLTYPE), _blobValue{nullptr, 0} {}
CellValue::CellValue(int iValue) : _type(Type::INTEGER), _iValue(iValue) {}
CellValue::CellValue(double dValue) : _type(Type::DOUBLE), _dValue(dValue) {}
CellValue::CellValue(const std::string& strValue) : _type(Type::TEXT) {
    _strValue = new char[strValue.size() + 1];
    std::strcpy(_strValue, strValue.c_str());
}
CellValue::CellValue(const char* cstrValue) : _type(Type::TEXT) {
    if(cstrValue){
        _strValue = new char[std::strlen(cstrValue) + 1];
        std::strcpy(_strValue, cstrValue);
    } else {
        _strValue = new char[1];
        _strValue[0] = '\0';
    }
}
CellValue::CellValue(std::nullptr_t) : _type(Type::NULLTYPE), _blobValue{nullptr, 0} {}
CellValue::CellValue(void* blob_value, int n) : _type(Type::BLOB) {
    _blobValue.data = new unsigned char[n];
    std::memcpy(_blobValue.data, blob_value, n);
    _blobValue.size = n;
}

CellValue::~CellValue() {
    if (_type == Type::TEXT) {
        delete[] _strValue;
    } else if (_type == Type::BLOB) {
        delete[] _blobValue.data;
    }
    _type = Type::NULLTYPE;
    _blobValue={nullptr, 0};
}

CellValue::CellValue(const CellValue& other):_type(other._type), _blobValue{nullptr, 0} {
    switch(_type){
        case Type::INTEGER:
            _iValue = other._iValue;
            break;
        case Type::DOUBLE:
            _dValue = other._dValue;
            break;
        case Type::TEXT:
            _strValue = new char[std::strlen(other._strValue) + 1];
            std::strcpy(_strValue, other._strValue);
            break;
        case Type::BLOB:
            _blobValue.size = other._blobValue.size;
            _blobValue.data = new unsigned char[_blobValue.size];
            std::memcpy(_blobValue.data, other._blobValue.data, _blobValue.size);
            break;
        case Type::NULLTYPE:
        default:
            _blobValue={nullptr, 0};
            break;
    }
}

CellValue::CellValue( CellValue&& other) noexcept : _type(other._type), _blobValue{nullptr, 0} {
    switch(_type){
        case Type::INTEGER:
            _iValue = other._iValue;
            break;
        case Type::DOUBLE:
            _dValue = other._dValue;
            break;
        case Type::TEXT:
            _strValue = other._strValue;
            break;
        case Type::BLOB:
            _blobValue.data = other._blobValue.data;
            _blobValue.size = other._blobValue.size;
            break;
        case Type::NULLTYPE:
        default:
            _blobValue={nullptr, 0};
            break;
    }
    other._type = Type::NULLTYPE;
    other._blobValue={nullptr, 0};
}

CellValue& CellValue::operator=(const CellValue& other) {
    if (this != &other) {
        // Clean up existing resources
        this->~CellValue();

        _type = other._type;
        switch(_type){
            case Type::INTEGER:
                _iValue = other._iValue;
                break;
            case Type::DOUBLE:
                _dValue = other._dValue;
                break;
            case Type::TEXT:
                _strValue = new char[std::strlen(other._strValue) + 1];
                std::strcpy(_strValue, other._strValue);
                break;
            case Type::BLOB:
                _blobValue.size = other._blobValue.size;
                _blobValue.data = new unsigned char[_blobValue.size];
                std::memcpy(_blobValue.data, other._blobValue.data, _blobValue.size);
                break;
            case Type::NULLTYPE:
            default:
                _blobValue={nullptr, 0};
                break;
        }
    }
    return *this;
}

CellValue& CellValue::operator=( CellValue&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        this->~CellValue();

        _type = other._type;
        switch(_type){
            case Type::INTEGER:
                _iValue = other._iValue;
                break;
            case Type::DOUBLE:
                _dValue = other._dValue;
                break;
            case Type::TEXT:
                _strValue = other._strValue;
                break;
            case Type::BLOB:
                _blobValue.data = other._blobValue.data;
                _blobValue.size = other._blobValue.size;
                break;
            case Type::NULLTYPE:
            default:
                _blobValue={nullptr, 0};
                break;
        }
        other._type = Type::NULLTYPE;
        other._blobValue={nullptr, 0};
    }
    return *this;
}

template<>
int CellValue::valueAs<int>() const {
    if(_type == Type::INTEGER){
        return _iValue;
    } else if(_type == Type::DOUBLE){
        return static_cast<int>(_dValue);
    } else if(_type == Type::TEXT){
        return std::stoi(_strValue);
    }
    return 0;
}

template<>
double CellValue::valueAs<double>() const {
    if(_type == Type::DOUBLE){
        return _dValue;
    } else if(_type == Type::INTEGER){
        return static_cast<double>(_iValue);
    } else if(_type == Type::TEXT){
        return std::stod(_strValue);
    }
    return 0.0;
}

template<>
std::string CellValue::valueAs<std::string>() const {
    if(_type == Type::TEXT){
        return std::string(_strValue);
    } else if(_type == Type::INTEGER){
        return std::to_string(_iValue);
    } else if(_type == Type::DOUBLE){
        return std::to_string(_dValue);
    }
    return "";
}

template<>
std::vector<uint8_t> CellValue::valueAs<std::vector<uint8_t>>() const {
    if(_type == Type::INTEGER){
        std::vector<uint8_t> vec(sizeof(int));
        std::memcpy(vec.data(), &_iValue, sizeof(int));
        return vec;
    } else if(_type == Type::DOUBLE){
        std::vector<uint8_t> vec(sizeof(double));
        std::memcpy(vec.data(), &_dValue, sizeof(double));
        return vec;
    } else if(_type == Type::TEXT){
        const char* strData = _strValue;
        size_t len = std::strlen(strData);
        return std::vector<uint8_t>(strData, strData + len);
    } else if(_type == Type::BLOB){
        return std::vector<uint8_t>(_blobValue.data, _blobValue.data + _blobValue.size);
    }
    return {};
}

std::ostream& operator<<(std::ostream& os, const CellValue& cellValue){
    switch(cellValue.valueType()){
        case CellValue::Type::INTEGER:
            os << cellValue.valueAs<int>();
            break;
        case CellValue::Type::DOUBLE:
            os << cellValue.valueAs<double>();
            break;
        case CellValue::Type::TEXT:
            os << cellValue.valueAs<std::string>();
            break;
        case CellValue::Type::BLOB: {
            std::vector<uint8_t> blobData = cellValue.valueAs<std::vector<uint8_t>>();
            os << "BLOB(" << blobData.size() << " bytes)";
            break;
        }
        case CellValue::Type::NULLTYPE:
            os << "NULL";
            break;
    }
    return os;
}


Statement::Binder& Statement::Binder::operator=(int value) {
    if(_index < 0){
        parentStmt.bindById(_id, value);
    } else {
        parentStmt.bind(value, _index);
    }
    return *this;
}
Statement::Binder& Statement::Binder::operator=(double value) {
    if(_index < 0){
        parentStmt.bindById(_id, value);
    } else {
        parentStmt.bind(value, _index);
    }
    return *this;
}
Statement::Binder& Statement::Binder::operator=(const std::string& value) {
    if(_index < 0){
        parentStmt.bindById(_id, value);
    } else {
        parentStmt.bind(value, _index);
    }
    return *this;
}
Statement::Binder& Statement::Binder::operator=(const char* value) {
    if(_index < 0){
        parentStmt.bindById(_id, value);
    } else {
        parentStmt.bind(value, _index);
    }
    return *this;
}
Statement::Binder& Statement::Binder::operator=(std::nullptr_t) {
    if(_index < 0){
        parentStmt.bindById(_id, nullptr);
    } else {
        parentStmt.bind(nullptr, _index);
    }
    return *this;
}
Statement::Binder& Statement::Binder::operator=(const std::vector<uint8_t>& blob_value) {
    if(_index < 0){
        parentStmt.bindById(_id, blob_value);
    } else {
        parentStmt.bind(blob_value, _index);
    }
    return *this;
}


Statement::Statement(std::shared_ptr<sqlite3> handle, const std::string& query) : 
    _stmt(nullptr), _query(query), _bindIndex(1), _rowIndex(0), _currentRow(0, nullptr) {
    if (handle) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(handle.get(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return;
        }
        _stmt = std::shared_ptr<sqlite3_stmt>(stmt, sqlite3_finalize);
    }
}

Statement::Statement() : 
    _stmt(nullptr), _query(""), _bindIndex(1), _rowIndex(0), _currentRow(0, nullptr) {}

Statement::Statement(Statement&& other) noexcept : 
    _stmt(std::move(other._stmt)), _query(std::move(other._query)), _bindIndex(other._bindIndex), 
    _rowIndex(other._rowIndex), _currentRow(std::move(other._currentRow)) {
    other._bindIndex = 1;
    other._rowIndex = 0;
}

Statement& Statement::operator=(Statement&& other) noexcept {
    if (this != &other) {
        _stmt = std::move(other._stmt);
        _query = std::move(other._query);
        _bindIndex = other._bindIndex;
        _rowIndex = other._rowIndex;
        _currentRow = std::move(other._currentRow);
        other._bindIndex = 1;
        other._rowIndex = 0;
        other._currentRow = Row(0, nullptr);
    }
    return *this;
}

Statement::~Statement() {
    // shared_ptr with custom deleter handles cleanup automatically
}

void Statement::reset(bool clearBindings) {
    if (isValid()) {
        sqlite3_reset(_stmt.get());
        if (clearBindings) {
            _bindIndex = 1;
            sqlite3_clear_bindings(_stmt.get());
        }
    }
    _rowIndex = 0;
    _currentRow = Row(0, nullptr);
}

void Statement::finalize() {
    if (isValid()) {
        _stmt.reset();
    }
}

Statement& Statement::bind(const std::string& value, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }
    
    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }

    int rc =  sqlite3_bind_text(_stmt.get(), _bindIndex++, value.c_str(), -1, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(const char* value, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }

    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }

    int rc = sqlite3_bind_text(_stmt.get(), _bindIndex++, value, -1, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(std::nullptr_t, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }

    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }
    int rc = sqlite3_bind_null(_stmt.get(), _bindIndex++);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(int value, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }
    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }
    int rc = sqlite3_bind_int(_stmt.get(), _bindIndex++, value);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(double value, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }

    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }

    int rc = sqlite3_bind_double(_stmt.get(), _bindIndex++, value);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(void* blob_value, int n, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }

    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }

    int rc = sqlite3_bind_blob(_stmt.get(), _bindIndex++, blob_value, n, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(const std::vector<uint8_t>& blob_value, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }

    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    }

    int rc = sqlite3_bind_blob(_stmt.get(), _bindIndex++, blob_value.data(), static_cast<int>(blob_value.size()), SQLITE_TRANSIENT);
    if(rc != SQLITE_OK){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }
    return *this;
}

Statement& Statement::bind(const CellValue& cellValue, int index) {
    if(!isValid()){
        throw DatabaseException(SQ3::NOMEM, "Cannot bind value: statement is not valid.");
    }
    switch(cellValue.valueType()){
        case CellValue::Type::INTEGER:
            return bind(cellValue.valueAs<int>(), index);
        case CellValue::Type::DOUBLE:
            return bind(cellValue.valueAs<double>(), index);
        case CellValue::Type::TEXT:
            return bind(cellValue.valueAs<std::string>(), index);
        case CellValue::Type::BLOB:
            return bind(cellValue.valueAs<std::vector<uint8_t>>(), index);
        case CellValue::Type::NULLTYPE:
        default:
            return bind(nullptr, index);
    }
}

Statement& Statement::bindById(const std::string& id, const std::string& value) {
    int index = getIndexForId(id);
    if(index < 0){
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(value, index);
}
Statement& Statement::bindById(const std::string& id, const char* value) {
    int index = getIndexForId(id);
    if(index < 0){
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(value, index);
}
Statement& Statement::bindById(const std::string& id, std::nullptr_t) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(nullptr, index);
}
Statement& Statement::bindById(const std::string& id, int value) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(value, index);
}
Statement& Statement::bindById(const std::string& id, double value) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(value, index);
}
Statement& Statement::bindById(const std::string& id, void* blob_value, int n) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(blob_value, n, index);
}
Statement& Statement::bindById(const std::string& id, const std::vector<uint8_t>& blob_value) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(blob_value, index);
}

Statement& Statement::bindById(const std::string& id, const CellValue& cellValue) {
    int index = getIndexForId(id);
    if(index < 0) {
        throw DatabaseException(SQ3::MISUSE, "Parameter name not found: " + id);
    }
    return bind(cellValue, index);
}

int Statement::getIndexForId(const std::string& id) {
    if(!isValid()){
        return -1;
    }
    // Try all SQLite parameter prefixes: :, @, $
    const char* prefixes[] = {":", "@", "$"};
    for(const char* prefix : prefixes) {
        std::string paramName = std::string(prefix) + id;
        int index = sqlite3_bind_parameter_index(_stmt.get(), paramName.c_str());
        if(index > 0) {
            return index - 1; // Convert to 0-based index
        }
    }
    return -1;
}

void Statement::resetBindIndex(int index) {
    if(index >= 0){
        _bindIndex = index + 1; // SQLite parameters are 1-based
    } else {
        _bindIndex = 1;
    }
}


SQ3 Statement::step(std::function<void(Row& row)> onRowFound) {
    if (!isValid()) {
        return SQ3::MISUSE;
    }
    int rc = sqlite3_step(_stmt.get());
    if(rc == SQLITE_ROW){
        _currentRow = Row(_rowIndex, _stmt);
        if(onRowFound){
            onRowFound(_currentRow);
        }
        ++_rowIndex;
    }
    return static_cast<SQ3>(rc);
}

int Statement::execute() {
    return execute(nullptr);
}


int Statement::execute(std::function<void(Row& row)> onRowFound){
    if(!isValid()) {
        throw DatabaseException(SQ3::MISUSE, "Cannot execute statement: statement is not valid.");
    }
    int rc = SQLITE_OK;
    bool isSelect = false;
    do{
        rc = sqlite3_step(_stmt.get());
        if(rc == SQLITE_ROW){
            _currentRow = Row(_rowIndex, _stmt);
            if(onRowFound){
                onRowFound(_currentRow);
            }
            ++_rowIndex;
            isSelect = true;
        }
    } while (rc == SQLITE_ROW);

    if(rc != SQLITE_DONE){
        const char* errMsg = sqlite3_errmsg(sqlite3_db_handle(_stmt.get()));
        if(!errMsg) errMsg = sqlite3_errstr(rc);
        throw DatabaseException(static_cast<SQ3>(rc), errMsg);
    }

    if(isSelect){
        return _rowIndex; // Number of rows retrieved
    }
    return sqlite3_changes(sqlite3_db_handle(_stmt.get()));
}
int Statement::execute(std::vector<std::vector<CellValue>>& outRows, std::vector<std::string>* outColumnNames){
    if(!isValid()) {
        return SQLITE_MISUSE;
    }
    outRows.clear();
    if(outColumnNames){
        outColumnNames->clear();
        int colCount = sqlite3_column_count(_stmt.get());
        for(int i=0; i<colCount; ++i){
            const char* colName = sqlite3_column_name(_stmt.get(), i);
            outColumnNames->push_back(colName ? std::string(colName) : "");
        }
    }  
    
    return execute([&outRows, outColumnNames](Row& row){
        std::vector<CellValue> currentRow;
        for(auto it=row.begin(); it!=row.end(); ++it){
            auto value = it->valueAs<CellValue>();
            currentRow.push_back(std::move(value));
        }
        outRows.push_back(std::move(currentRow));
    });    
}

Row::Row(int rowIndex, std::shared_ptr<sqlite3_stmt> stmt) : _rowIndex(rowIndex),_columnCount(0), _stmt(stmt) {
    if(_stmt){
        _columnCount = sqlite3_column_count(_stmt.get());
    }
}

Row::Row(Row&& other) : _rowIndex(other._rowIndex), _columnCount(other._columnCount), _stmt(std::move(other._stmt)) {
    other._rowIndex = 0;
    other._columnCount = 0;
    other._stmt = nullptr;
}

Row& Row::operator=(Row&& other) {
    if (this != &other) {
        _rowIndex = other._rowIndex;
        _columnCount = other._columnCount;
        _stmt = std::move(other._stmt);

        other._rowIndex = 0;
        other._columnCount = 0;
        other._stmt = nullptr;
    }
    return *this;
}

Row::Cell::Cell(int columnIndex, Row* row) : _column(columnIndex), _parent(row) {
    if(_parent){
        if(columnIndex >= _parent->getColumnCount() || columnIndex < 0){
            // End iterator or invalid index
            _column = -1;
            _type = SQLITE_NULL;
        }else{
            _type = sqlite3_column_type(_parent->_stmt.get(), columnIndex);
        }
    }else{
        _type = SQLITE_NULL;
        _column = -1;
    }
}

Row::Cell::Cell(Row::Cell&& other) : _column(other._column), _type(other._type), _parent(other._parent) {
    other._column = -1;
    other._type = SQLITE_NULL;
    other._parent = nullptr;
}

Row::Cell& Row::Cell::operator=(Row::Cell&& other) {
    if (this != &other) {
        _column = other._column;
        _type = other._type;
        _parent = other._parent;

        other._parent = nullptr;
        other._column = -1;
        other._type = SQLITE_NULL;
    }
    return *this;
}


bool Row::Cell::isNull() const {
    return _type == SQLITE_NULL;
}

template<>
std::string Row::Cell::valueAs<std::string>(bool autoConvert) const {
    const char* txt = nullptr;
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }

    if(!autoConvert && _type != SQLITE_TEXT){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: type mismatch.");
    }

    txt = reinterpret_cast<const char*>(sqlite3_column_text(_parent->_stmt.get(), _column));
    return txt ? std::string(txt) : "";
}

template<>
const char* Row::Cell::valueAs<const char*>(bool autoConvert) const {
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }

    if(!autoConvert && _type != SQLITE_TEXT){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: type mismatch.");
    }

    return reinterpret_cast<const char*>(sqlite3_column_text(_parent->_stmt.get(), _column));
}

template<>
int Row::Cell::valueAs<int>(bool autoConvert) const {
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }
    if(!autoConvert && _type != SQLITE_INTEGER){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: type mismatch.");
    }
    return sqlite3_column_int(_parent->_stmt.get(), _column);
}

template<>
double Row::Cell::valueAs<double>(bool autoConvert) const {
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }
    if(!autoConvert && _type != SQLITE_FLOAT){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: type mismatch.");
    }
    return sqlite3_column_double(_parent->_stmt.get(), _column);
}

template<>
std::vector<uint8_t> Row::Cell::valueAs<std::vector<uint8_t>>(bool autoConvert) const {
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }
    if(!autoConvert && _type != SQLITE_BLOB){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: type mismatch.");
    }
    const void* blobData = sqlite3_column_blob(_parent->_stmt.get(), _column);
    int size = sqlite3_column_bytes(_parent->_stmt.get(), _column);
    if(blobData && size > 0){
        const uint8_t* dataPtr = static_cast<const uint8_t*>(blobData);
        return std::vector<uint8_t>(dataPtr, dataPtr + size);
    } else {
        return {};
    }
}

template<>
CellValue Row::Cell::valueAs<CellValue>(bool autoConvert) const {
    (void)autoConvert; // Currently not used
    if(isNull() || !_parent || _column < 0){
        throw DatabaseException(SQ3::MISUSE, "Cannot retrieve value: invalid cell.");
    }

    switch(_type){
        case SQLITE_INTEGER: {
            int intValue = sqlite3_column_int(_parent->_stmt.get(), _column);
            return CellValue(intValue);
        }
        case SQLITE_FLOAT: {
            double doubleValue = sqlite3_column_double(_parent->_stmt.get(), _column);
            return CellValue(doubleValue);
        }
        case SQLITE_TEXT: {
            const char* textValue = reinterpret_cast<const char*>(sqlite3_column_text(_parent->_stmt.get(), _column));
            return CellValue(textValue ? textValue : "");
        }
        case SQLITE_BLOB: {
            const void* blobData = sqlite3_column_blob(_parent->_stmt.get(), _column);
            int size = sqlite3_column_bytes(_parent->_stmt.get(), _column);
            return CellValue(const_cast<void*>(blobData), size);
        }
        case SQLITE_NULL:
        default:
            return CellValue();
    }
}


const char* Row::Cell::columnName() const {
    if(_parent && _column >=0){
        const char* colName = sqlite3_column_name(_parent->_stmt.get(), _column);
        return colName ? colName : "";
    }
    return "";
}


Row::iterator Row::begin() {
    if(!_stmt){
        throw DatabaseException(SQ3::MISUSE, "Statement is not valid.");
    }
    return iterator(0, *this);
}

Row::iterator Row::end() {
    if(!_stmt){
        throw DatabaseException(SQ3::MISUSE, "Statement is not valid.");
    }
    return iterator(_columnCount, *this);
}

CellValue Row::valueAt(int columnIndex){
    if(!_stmt){
        throw DatabaseException(SQ3::MISUSE, "Statement is not valid.");
    }

    if(columnIndex < 0 || columnIndex >= _columnCount){
        throw DatabaseException(SQ3::MISUSE, "Column index out of range.");
    }
    return Cell(columnIndex, this).valueAs<CellValue>();
}

CellValue Row::operator[](int columnIndex){
    return valueAt(columnIndex);
}

CellValue Row::operator[](const std::string& colName){
    if(!_stmt){
        throw DatabaseException(SQ3::MISUSE, "Statement is not valid.");
    }
    for(auto it=begin(); it!=end(); ++it){
        const char* currentColName = sqlite3_column_name(_stmt.get(), it->_column);
        if(currentColName && colName == std::string(currentColName)){
            return it->valueAs<CellValue>();
        }
    }
    throw DatabaseException(SQ3::MISUSE, "Column name not found: " + colName);
}


Row::iterator::iterator(int index, Row& row) : _index(index), _row(row),_cell(index, &row) {
}

Row::iterator& Row::iterator::operator++() {
    ++_index;
    _cell = Row::Cell(_index, &_row);
    return *this;
}
bool Row::iterator::operator!=(const iterator& other) {
    return _index != other._index;
}

const Row::Cell& Row::iterator::operator*()  const{
    // Row::Cell Cell(_index, _row);
    // const char* colName = sqlite3_column_name(_row._stmt, _index);
    // const char* colText = reinterpret_cast<const char*>(sqlite3_column_text(_row._stmt, _index));
    // Cell.column = colName ? std::string(colName) : "";
    // if (colText) {
    //     Cell.value = std::string(colText);
    //     Cell.isNull = false;
    // } else {
    //     Cell.value = "";
    //     Cell.isNull = true;
    // }
    return _cell;
}

std::ostream& operator<<(std::ostream& os, const sq3pp::Row::Cell& cell){
    if(!cell){
        os << "NULL";
    } else {
        switch(cell.valueType()){
            case SQLITE_INTEGER:
                os << cell.valueAs<int>();
                break;
            case SQLITE_FLOAT:
                os << cell.valueAs<double>();
                break;
            case SQLITE_TEXT:
                os << cell.valueAs<const char*>();
                break;
            case SQLITE_BLOB: {
                std::vector<uint8_t> blobData = cell.valueAs<std::vector<uint8_t>>();
                os << "BLOB(" << blobData.size() << " bytes)";
                break;
            }
            case SQLITE_NULL:
            default:
                os << "NULL";
                break;
        }
    }
    return os;
}