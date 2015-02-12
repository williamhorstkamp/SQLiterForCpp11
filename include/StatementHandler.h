/**
 *  StatementHandler.h
 *  Provides a wrapper for a unique_ptr<sqlite3_stmt> that both
 *  cleanly prepares the statement and manages the lifetime of it
 *  with the bonus of providing a convenient interface
 *
 *  @author William Horstkamp
 */

#include <sqlite3.h>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "ValueHandler.h"

namespace SQLiter {

    /**
     *  Custom deleter for unique_ptr<sqlite3_stmt>'s raw pointer
     *  implemented as an object as function
     *
     *  @param stmt - the sqlite3_stmt raw pointer to be deleted
     */
    struct Closesqlite3_stmt {
        void operator()(sqlite3_stmt *stmt) const {
            sqlite3_finalize(stmt);
        }
    };

    /**
     *  Class that manages an sqlite3_stmt unique_ptr
     *  This class is used for its move and copy operators and as
     *  a convenience wrapper to allow C++11 functionality to be used
     *  with SQLite3's C interface
     */
    class StatementHandler {
    private:

        std::unique_ptr<sqlite3_stmt, Closesqlite3_stmt> stmt;
        std::map<const std::string , int> inputAlias;
        std::map<const std::string , int> outputAlias;
    public:

        /**
        *  Constructor prepares a statement on a db given a db and string.
        *
        *  @param db - Database to prepare the statement on
        *  @param stmtStr - string to build the prepared statement with
        *
        *  @return - StatementHandler containing a unique_ptr that is managing
        *      a prepared statement that is ready to be bound
        */
        StatementHandler(sqlite3 *db, const std::string stmtStr);

        /**
         *  Default Destructor cleans up our sqlite3_stmt unique_ptr and clears
         *  both the inputAlias and outputAlias maps.
         */
        ~StatementHandler();

        /**
         *  Explicitely deletes the copy constructor to match the abilities
         *  of unique_ptr.
         */
        StatementHandler(StatementHandler const &) = delete;

        /**
         *  Explicitely deletes the copy assignment operator to match the
         *  abilities of unique_ptr.
         */
        StatementHandler &operator=(StatementHandler const &) = delete;

        /**
         *  Deletes the move assignment operator to prevent the user from
         *  taking the prepared statement from the StatementHandler.
         */
        StatementHandler &operator=(StatementHandler &&o) = delete;

        /**
         *  Deletes the move constructor to prevent the user from taking the
         *  prepared statement from the StatementHandler.
         */
        StatementHandler(StatementHandler &&o) = delete;

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a std::string  as input.
         *
         *  @param var - Input column as int
         *      Begins with 1, as per the SQLite standard
         *  @param input - pointer to C string to bind
         */
        void bind(const int var, const std::string input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to an int as input.
         *
         *  @param var - Input column as int
         *      Begins with 1, as per the SQLite standard
         *  @param input - int to bind
         */
        void bind(const int var, const int input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a double as input.
         *
         *  @param var - Input column as int
         *      Begins with 1, as per the SQLite standard
         *  @param input - double to bind
         */
        void bind(const int var, const double input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a blob as input.
         *
         *  @param var - Input column as int
         *      Begins with 1, as per the SQLite standard
         *  @param input - blob to bind
         */
        void bind(const int var, const void *input, const int size);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to null.
         *
         *  @param int var - Column to be set to null as int
         */
        void bindNull(const int var);

        /**
         *  Binds the variable with a given alias in the prepared statement
         *  to a std::string  as input.
         *
         *  @param var - the alias of the prepared statement that the input is for.
         *  @param input - pointer to C string to bind
         */
        inline void bind(const std::string var, const std::string input) {
            bind(inputAlias.at(var), input);
        }

        /**
         *   Binds the variable with a given alias in the prepared statement
         *  to an int as input.
         *
         *  @param var - the alias of the prepared statement that the input is for
         *  @param input - int to bind
         */
        inline void bind(const std::string var, const int input) {
            bind(inputAlias.at(var), input);
        }

        /**
         *   Binds the variable with a given alias in the prepared statement
         *  to a double as input.
         *
         *  @param var - the alias of the prepared statement that the input is for
         *  @param input - double to bind
         */
        inline void bind(const std::string var, const double input) {
            bind(inputAlias.at(var), input);
        }

        /**
         *  Binds the variable with a given alias in the prepared statement
         *  to a blob as .
         *
         *  @param var - the alias of the prepared statement that the input is for
         *  @param input - blob to bind
         */
        inline void bind(const std::string var, const void *input, const int size) {
            bind(inputAlias.at(var), input, size);
        }

        /**
         *  Binds the variable with a given alias in the prepared statement
         *  to null.
         *
         *  @param var - the alias of the prepared statement that is to be
         *      set to null
         */
        inline void bindNull(const std::string var) {
            sqlite3_bind_null(stmt.get(), inputAlias.at(var));
        }

        /**
         *  Gives the return type of a resultant column as integer.
         *
         *  @param column - Integer representing column whose type to check
         *
         *  @return int - Integer containing the corresponding SQLite3 datatype code
         *      Possible results: 1 - INT, 2 - FLOAT, 3 - TEXT, 4 - BLOB, 5 - NULL
         *      0 - ERROR
         */
        const int getType(const int column);

        /**
         *  Returns the size of the item currently in the result column.
         *
         *  @param column - Integer representing column whose type to check
         *
         *  @return - Integer representing the size of the value currently in the
         *      the column, in bytes.
         */
        const int getSize(const int column);

        /**
         *  Returns a pointer to a null-terminated string in the specified column
         *  of the result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant text from
         *
         *  @return const char* - pointer to the null terminated character array
         *      containing resultant text for a given column or nullptr if column
         *      contains null
         */
        const std::string getString(const int column);

        /**
         *  Returns an integer in the specified column of the result of the
         *  latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *
         *  @return int - integer value
         */
        const int getInt(const int column);

        /**
         *  Returns an SQLite3 int64 in the specified column of the result of the
         *  latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *
         *  @return int - sqlite3_int64 value
         */
        const sqlite3_int64 getInt64(const int column);

        /**
         *  Returns a double precision float in the specified column of the
         *  result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *
         *  @return double - double value
         */
        const double getDouble(const int column);

        /**
         *  Returns a blob in the specified column of the
         *  result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *
         *  @return - pointer to blob
         */
        const void *getBlob(const int column);

        /**
         *  Returns a ValueHandler objects that functions as a wrapper and
         *  type converter for the range of return types that SQLite supports.
         *  Useful if you want to be able to implicitly cast from a column.
         *
         *  Useage:     int var = stmt.getColumn(1);
         *              double var2 = stmt.getColumn(2);
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant column value from
         *
         *  @return - ValueHandler that wraps an SQLite3 value
         */
        const ValueHandler getColumn(const int column);

        /**
         *  Gives the return type of a resultant column as integer.
         *
         *  @param column - C String representing column whose type to check
         *
         *  @return int - Integer containing the corresponding SQLite3 datatype code
         *      Possible results: 1 - INT, 2 - FLOAT, 3 - TEXT, 4 - BLOB, 5 - NULL
         *      0 - ERROR
         */
        inline const int getType(const std::string column) {
            return getType(outputAlias.at(column));
        }

        /**
         *  Returns the size of the item currently in the result column.
         *
         *  @param column - C String representing column whose type to check
         *
         *  @return - Integer representing the size of the value currently in the
         *      the column, in bytes.
         */
        inline const int getSize(const std::string column) {
            return getSize(outputAlias.at(column));
        }

        /**
         *  Returns a pointer to a null-terminated string in the specified column
         *  of the result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - C String representing the column number to pull the
         *      resultant text from
         *
         *  @return const char* - pointer to the null terminated character array
         *      containing resultant text for a given column or nullptr if column
         *      contains null
         */
        inline std::string getString(const std::string column) {
            return getString(outputAlias.at(column));
        }

        /**
         *  Returns an integer in the specified column of the result of the
         *  latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - C String representing the column number to pull the
         *      resultant int from
         *
         *  @return int - integer value
         */
        inline int getInt(const std::string column) {
            return getInt(outputAlias.at(column));
        }

        /**
         *  Returns an SQLite3 int64 in the specified column of the result of the
         *  latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - C String representing the column number to pull the
         *      resultant int from
         *
         *  @return int - sqlite3_int64 value
         */
        inline sqlite3_int64 getInt64(const std::string column) {
            return getInt64(outputAlias.at(column));
        }

        /**
         *  Returns a double precision float in the specified column of the
         *  result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - C String representing the column number to pull the
         *      resultant int from
         *
         *  @return double - double value
         */
        inline double getDouble(const std::string column) {
            return getDouble(outputAlias.at(column));
        }

        /**
         *  Returns a blob in the specified column of the
         *  result of the latest step.
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - C String representing the column number to pull the
         *      resultant int from
         *
         *  @return - pointer to blob
         */
        inline const void *getBlob(const std::string column) {
            return getBlob(outputAlias.at(column));
        }

        /**
         *  Returns a ValueHandler objects that functions as a wrapper and
         *  type converter for the range of return types that SQLite supports.
         *  Useful if you want to be able to implicitly cast from a column.
         *
         *  Useage:     int var = stmt.getColumn("col 1");
         *              double var2 = stmt.getColumn("another column");
         *
         *  @param column - C String representing the column number to pull the
         *      resultant column value from
         *
         *  @return - ValueHandler that wraps an SQLite3 value
         */
        inline ValueHandler getColumn(const std::string column) {
            return getColumn(outputAlias.at(column));
        }

        /**
         *  Steps the prepared statement a single time.
         *
         *  @return bool - Returns true if there are results from the SQL
         *      statement as a result of stepping. Further calls to step or reset
         *      will cause a loss of these results if they are not read using one
         *      of the column readers.
         */
        const bool step();

        /**
         *  Resets the prepared statement so it is ready to executed again.
         */
        void reset();

        /**
         *  Clears the bindings of a prepared statement by setting them to null.
         *  Not necissary but useful if some fields can/should be null.
         *  The SQLite documentation made special mention of the fact that to 
         *  reset and to clear bindings are different functions in their entirety.
         */
        void clear();

        /**
         *  Function for counting the number of columns returned by
         *  prepared statement.
         *
         *  @return - number of columns returned by the statement
         */
        const int columnCount();

        /**
         *  Returns the name of the database the statement column is from.
         *
         *  @param - Integer representing column to lookup
         *
         *  @return - Pointer to null terminated C String containing database
         *      name.
         */
        const std::string databaseName(const int col);

        /**
         *  Returns the name of the table the statement column is from.
         *
         *  @param - Integer representing column to lookup
         *
         *  @return - Pointer to null terminated C String containing name of
         *      the table.
         */
        const std::string tableName(const int col);

        /**
         *  Returns the name of the column the statement column is from.
         *       This is in reference to the name as stored on the table,
         *       as opposed to prepared statement's field as set by the user.
         * 
         *  @param - Integer representing column to lookup
         *
         *  @return - Pointer to null terminated C String containing column
         *      name.
         */
        const std::string columnName(const int col);

        /**
         *  Returns the name of the database the statement column is from
         *
         *  @param - C String representing column alias to lookup
         *
         *  @return - Pointer to null terminated C String containing database
         *      name.
         */
        inline const std::string databaseName(const std::string col) {
            return databaseName(outputAlias.at(col));
        }

        /**
         *  Returns the name of the table the statement column is from.
         *
         *  @param - C String representing column alias to lookup
         *
         *  @return - Pointer to null terminated C String containing name of
         *      the table.
         */
        inline const std::string tableName(const std::string col) {
            return tableName(outputAlias.at(col));
        }

        /**
         *  Returns the name of the column the statement column is from.
         *       This is in reference to the name as stored on the table,
         *       as opposed to prepared statement's field as set by the user.
         *
         *  @param - C String representing column alias to lookup.
         *
         *  @return - Pointer to null terminated C String containing column
         *      name.
         */
        inline const std::string columnName(const std::string col) {
            return columnName(outputAlias.at(col));
        }

        /**
         *  Binds an input column to an alias.
         *
         *  @param alias - C String to be used as the alias to the input
         *      column
         *  @param colNum - Integer representing the input column to bind to
         *      the alias
         */
        void setInputAlias(const std::string alias, const int colNum);

        /**
         *  Binds an output column to an alias.
         *
         *  @param alias - C String to be used as the alias to the input
         *      column
         *  @param colNum - Integer representing the output column to bind to
         *      the alias
         */
        void setOutputAlias(const std::string alias, const int colNum);
    };
}