/**
 *  SQLiteHandler.h
 *  Provides a portable SQLite3 interface to be used with the StatementHandler
 *
 *  @author William Horstkamp
 *  @version 0.6
 */
#include <sqlite3.h>
#include <string>
#include <sys/stat.h>
#include <memory>
#include <map>
#include "StatementHandler.h"
#include "SQLiteException.h"

#ifndef SQLITEHANDLER_H
#define SQLITEHANDLER_H

/**
 *  Custom deleter for unique_ptr<sqlite3>'s raw pointer
 *  implemented as an object as function
 *
 *  @param *sql - the sqlite3 raw pointer to be deleted
 */
struct Closesqlite3 {
    void operator()(sqlite3 * sql) const {
        sqlite3_close(sql);
    }
};

/**
 *  Class that manages an sqlite3 db and map of strings->StatementHandler to
 *  provide a flexible and simple C++11 powered wrapper for the SQLite3 C 
 *  interface
 */
class SQLiteHandler {
private:

    std::unique_ptr<sqlite3, Closesqlite3> db;

    /**
     *  Map from string to unique_ptr<StatementHandler>
     *
     *  Allows the user to essentially 'name' a prepared statement which,
     *  along with the creation and cleanup functions, as well as the simple
     *  interface aids in useability and maintainability of a set of prepared
     *  statements. The StatementHandler is managed by a unique_ptr which
     *  allows for simple deletion by the SQLiteHandler and derived classes
     *  using map.clear() and map.erase(keytype keyvalue)
     */
    std::map<std::string, std::unique_ptr<StatementHandler>> stmts;

public:
    /**
     *  Default constructor
     */
    SQLiteHandler() {};

    /**
     *  Constructor takes a file location and opens the database file at that
     *  location. Does not check whether the file exists or not before opening,
     *  providing similiar functionality to the official C interface.
     *
     *  @param location - Pointer to null terminated C string containing the
     *      location to create/open the file from
     *
     *  @return - SQLiteHandler object managing the database referenced
     */
    SQLiteHandler(const char *location);

    /**
     *  Default destructor destroys prepared statements and resets the
     *  unique_ptr to the sqlite3 database, effectively destroying the object
     */
    ~SQLiteHandler();

    /**
     *  Takes a file location and opens the database file at that
     *  location. Does not check whether the file exists or not before opening,
     *  providing similiar functionality to the official C interface.
     *
     *  @param location - Pointer to null terminated C string containing the
     *      location to create / open the file from
     *
     *  @return -SQLiteHandler object managing the database referenced
     */
    void forceOpenDatabase(const char *location);

    /**
     *  Creates a new SQLite3 database at a given path and opens it, if a
     *  nullptr is given instead of a name, the datase will be place in memory
     *
     *  @param location - Location on disk to create the SQLite3 database.
     *      Any file extension may be used as SQLite3 does not have a default
     *      extension.
     *      If nullptr is entered instead of a pointer to a location on disk,
     *      the database will instead be created solely in memory
     *
     *  @return - SQLiteHandler object managing the database referenced
     */
    void createDatabase(const char *location);

    /**
     *  Open the database file at a given path
     *
     *  @param location - Location on disk from which to open the database
     *
     *  @return - SQLiteHandler object managing the database referenced
     */
    void openDatabase(const char *location);

    /**
     *  Closes the active SQLite3 database
     */
    void closeDatabase();

    /**
     *  Inline helper function checks if a file exists or not using stat()
     *
     *  @param location - location on disk to check for the existence of a file
     *
     *  @return - Boolean representing whether the file exists or not
     */
    inline bool fileExists(const char *location) {
        struct stat st;
        return (stat(location, &st) == 0);
    }

    /**
     *  Creates a StatementHandler based on a given input string, 
     *  prepares the statement, and places it in our statement map with
     *  a given key
     *
     *  @param key - Key to use to reference prepared statement
     *  @param stmtStr - String to use to build the prepared statement with.
     *      Must be in standard SQLite3 format
     */
    StatementHandler *prepareStatement(const char *key, const char *stmtStr);
 
    /**
     *  Returns a pointer to a StatementHandler given its key.
     *  This is used as a way to access the statement's various functions.
     *
     *  @param key - C string containing the key to the Statement
     *
     *  @return - Pointer to the statement's StatementHandler
     */
    StatementHandler *getStatement(const char *key);

    /**
     *  Destroys all prepared statements by clearing the statement map
     *  Can be called to clear all prepared statements and is called by the
     *  SQLiteHandler destructor to prevent memory leaks.
     */
    void destroyStatements();

    /**
     *  Executes an SQLite3 statement without first preparing it and binding it
     *  
     *  @param input - location to null terminated character string containing
     *      the SQL statement to be executed
     *
     *  @return - Number of rows modified
     */
    int rawExec(const char *stmtStr); 

    /**
     *  Checks to make sure the result code returned by an SQLite3 function
     *  call is SQLITE_OK, otherwise throws an exception containing the
     *  resultant error message.
     *
     *  @param resCode - Integer containing the SQLite3 result code in integer
     *  form, as returned by a function. 0 is SQLITE_OK, which is what is
     *  expected.
     */
    void result(const int resCode);

    /**
     *  Creates a scalar SQLite function using a set of C/C++ functions
     *
     *  @param name - Pointer to null terminated c string containing the
     *      function  name
     *  @param nArg - Number of arguments that that SQL function takes
     *  @param pApp - Arbitary pointer that can be used by the function calls
     *  @param xFunc - Function that is called on the input given by SQLite
     *  @param xDestroy - Destructor for pApp
     */
    void scalarFunction(const char *name, int nArg, void *pApp,
        void(*xFunc)(sqlite3_context*, int, sqlite3_value**),
        void(*xDestroy)(void*));

    /**
     *  Creates an aggregate SQLite function using a set of C/C++ functions
     *
     *  @param name - Pointer to null terminated c string containing the
     *      function  name
     *  @param nArg - Number of arguments that that SQL function takes
     *  @param pApp - Arbitary pointer that can be used by the function calls
     *  @param xStep - Function called on input given by SQLite for each row
     *  @param xFinal - Function to finalize aggregate
     *  @param xDestroy - Destructor for pApp
     */
    void aggregateFunction(const char *name, int nArg, void *pApp,
        void(*xStep)(sqlite3_context*, int, sqlite3_value**),
        void(*xFinal)(sqlite3_context*),
        void(*xDestroy)(void*));

    /**
     *  Function deletes a user created function by name
     *
     *  @param name - Pointer to null terminated c string containing function
     *      name to delete
     */
    void deleteFunction(const char *name);
};

#endif