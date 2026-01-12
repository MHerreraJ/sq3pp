#include <sq3pp/Database.h>
#include <sq3pp/Statement.h>
#include <sq3pp/Transaction.h>
#include <sq3pp/Exception.h>


#include <vector>
#include <iostream>
#include <random>

std::string randomName(){
    static const std::string names[] = {
        "Alice", "Bob", "Charlie", "Diana", "Eve",
        "Frank", "Grace", "Hannah", "Ivan", "Judy"
    };
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<std::size_t> dist(0, sizeof(names)/sizeof(names[0]) - 1);
    return names[dist(rng)];
}


int main(int argc, char* argv[]) {
    sq3pp::Database db;
    int rc = db.open("test.db");
    if (rc) {
        std::cerr << "Failed to open database: " << sqlite3_errstr(rc) << std::endl;
        return rc;
    }

    // Create a table and insert some data within a transaction
    try{
        sq3pp::Transaction t = db.beginTransaction();

        sq3pp::Statement st = db.createStatement("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER);");
        st.execute();

        for (int i = 0; i < 10; ++i) {
            sq3pp::Statement insertStmt = db.createStatement("INSERT INTO users (name, age) VALUES (:name, :age);");
            insertStmt["name"] = randomName();
            insertStmt["age"] = 20 + i * 3;
            insertStmt.execute();
        }
        std::cout << "Inserted 10 users into the database." << std::endl;
        t.commit();
    }catch(const sq3pp::DatabaseException& ex){
        std::cerr << "Database error (" << static_cast<int>(ex.code()) << "): " << ex.what() << std::endl;
        return static_cast<int>(ex.code());
    }catch(const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return -1;
    }


    // Query and display the data
    try{
        sq3pp::Statement queryStmt = db.createStatement("SELECT age, name FROM users WHERE age >= ? LIMIT 10;");
        queryStmt.bind(30);

        int rows= queryStmt.execute([](sq3pp::Row& row){
            //row[0] is the first column (age), row["name"] is the column named "name"
            std::cout << "Name: " << row["name"] << " is " << row[0] << " years old." << std::endl;
        });
        std::cout << "Retrieved " << rows << " rows with age >= 30." << std::endl;
    }catch(const sq3pp::DatabaseException& ex){
        std::cerr << "Database error (" << static_cast<int>(ex.code()) << "): " << ex.what() << std::endl;
        return static_cast<int>(ex.code());
    }catch(const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return -1;
    }

    try{
        sq3pp::Statement removeStmt = db.createStatement("DELETE FROM users WHERE age < ?;");
        removeStmt.bind(30);
        int affected = removeStmt.execute();
        std::cout << "Deleted " << affected << " rows where age < 30." << std::endl;
    }catch(const sq3pp::DatabaseException& ex){
        std::cerr << "Database error (" << static_cast<int>(ex.code()) << "): " << ex.what() << std::endl;
        return static_cast<int>(ex.code());
    }catch(const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}