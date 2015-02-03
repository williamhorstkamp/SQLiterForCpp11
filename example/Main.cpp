#include "SQLiteHandler.h"
#include <iostream>
#include <fstream>
#include <cstddef>

using namespace SQLiter;
int main(int argc, char *argv[])
{
    SQLiteHandler db("test.db3");  //makes an SQLite3 database named test.db3 if it doesn't exist, opens it either way
    
    const char *sql = "CREATE TABLE IF NOT EXISTS TestTable("
        "testtext TEXT PRIMARY KEY,"
        "testreal REAL"
        ");"
        "CREATE TABLE IF NOT EXISTS TestTable2("
        "testint	INTEGER PRIMARY KEY AUTOINCREMENT,"
        "testreal2 REAL NOT NULL,"
        "testblob BLOB NOT NULL,"
        "FOREIGN KEY(testreal2) REFERENCES TestTable(testreal)"
        ");";
    db.rawExec(sql);
    db.prepareStatement("testStatement", "INSERT INTO TestTable values (?, ?)");    //the following lines show the 'ugly' way of working with prepared statements
    db.getStatement("testStatement")->bind(1, "A String");  //binding statements
    db.getStatement("testStatement")->bind(2, 5.0);
    db.getStatement("testStatement")->step();   //stepping the statement
    db.getStatement("testStatement")->reset();  //resetting the statement to be ran again

    std::ifstream file("example/smile.png");    //opening a file into an ifstream
    file.seekg(0, std::ifstream::end);
    std::streampos size = file.tellg();
    file.seekg(0);
    char *buffer = new char[size];  //relatively unsafe - can crash the program if file is null
    file.read(buffer, size);
    


    StatementHandler *stmt = db.prepareStatement("testStatement2", 
        "INSERT INTO TestTable2(testreal2, testblob) values (?, ?)");    //these lines show the 'clean' way of working with prepared statements
    stmt->setInputAlias("real", 1);    //giving the input columns aliases
    stmt->setInputAlias("blob", 2);
    stmt->bind("real", 5.0); //does the same as stmt->bind(1, 5.0) but is easier to interpret
    stmt->bind("blob", buffer);
    stmt->step();
    stmt->reset();
    std::cout << db.changes() << " change(s)" << std::endl; // reporting the changed rows that the statement caused
    
    file.close();   //file cleanup
    delete buffer;
    
    
    db.prepareStatement("testStatement3",
        "select testtable.testtext, testTable.testreal, testTable2.testblob "
        "from testTable join testTable2 on "
        "testTable.testreal = testtable2.testreal2");
    stmt = db.getStatement("testStatement3");
    stmt->setOutputAlias("text", 0);   //giving the output columns aliases
    stmt->setOutputAlias("real as int", 1);
    stmt->setOutputAlias("blob", 2);
    while (stmt->step()) {  //getting back results from a prepared statement
        std::cout << stmt->getString("text") << "\t";    //calling get<type> is the best way to output directly into streams
        int i = stmt->getColumn("real as int"); //getColumn() is safe and easy so long as you are sure you are using the correct type
        std::cout << i << "\t";
        std::cout << stmt->getSize("blob") << std::endl;    //demonstrating a few functions
        std::cout << stmt->databaseName("text") << " " << stmt->tableName(2) << " ";
        std::cout << stmt->columnName("real as int") << " " << stmt->columnName(2);
        std::cout << " " << stmt->columnCount() << std::endl;
    }
    stmt->reset();
    db.destroyStatement("testStatement3");  //destroys a statement correctly (finalization) by its name

    std::cin.get();
    db.closeDatabase(); //the only cleanup required to get rid of the database and prepared statements correctly

    return 0;
}