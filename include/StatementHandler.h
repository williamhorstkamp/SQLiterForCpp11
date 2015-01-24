/**
 *  StatementHandler.h
 *  Provides a wrapper for a unique_ptr<sqlite3_stmt> that both
 *  cleanly prepares the statement and manages the lifetime of it
 *  with the bonus of providing a convenient interface
 *
 *  @author William Horstkamp
 *  @version 0.5
 */

#include <sqlite3.h>
#include <memory>
#include <vector>
#include <string>
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
    public:

        /**
         *  Default Destructor cleans up our sqlite3_stmt unique_ptr
         */
        ~StatementHandler();

        /**
         *  Explicitely deletes the copy constructor to match the abilities
         *  of unique_ptr
         */
        StatementHandler(StatementHandler const &) = delete;

        /**
         *  Explicitely deletes the copy assignment operator to match the
         *  abilities of unique_ptr
         */
        StatementHandler &operator=(StatementHandler const &) = delete;

        /**
         *  Deletes the move assignment operator to prevent the user from
         *  taking the prepared statement from the StatementHandler
         */
        StatementHandler &operator=(StatementHandler &&o) = delete;

        /**
         *  Deletes the move constructor to prevent the user from taking the
         *  prepared statement from the StatementHandler
         */
        StatementHandler(StatementHandler &&o) = delete;

        /**
         *  Constructor prepares a statement on a db
         *
         *  @param db - database to prepare the statement on
         *  @param stmtStr - string to build the prepared statement with
         *  @return - StatementHandler containing a unique_ptr that is managing
         *  a prepared statement that is ready to be bound
         */
        StatementHandler(sqlite3 *db, const char *stmtStr);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a char * as input
         *
         *  @param var - the place of the prepared statement that the input is for.
         *      Begins with 1, as per the SQLite standard
         *  @param char input - pointer to C string to bind
         */
        void bind(const int var, const char *input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to an int as input
         *
         *  @param int var - the place of the prepared statement that the input is for
         *      Begins with 1, as per the SQLite standard
         *
         *  @param int input - int to bind
         */
        void bind(const int var, const int input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a double as input
         *
         *  @param int var - the place of the prepared statement that the input is for
         *      Begins with 1, as per the SQLite standard
         *
         *  @param double input - double to bind
         */
        void bind(const int var, const double input);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to a blob as input
         *
         *  @param int var - the place of the prepared statement that the input is for
         *      Begins with 1, as per the SQLite standard
         *
         *  @param void input - blob to bind
         */
        void bind(const int var, const void *input, const int size);

        /**
         *  Binds the variable in a given position of the prepared statement
         *  to null
         *
         *  @param int var - the place of the prepared statement that is to be
         *      set to null
         */
        void bindNull(const int var);

        /**
        *  Gives the return type of a resultant column as integer
        *
        *  @param column - Integer representing column whose type to check
        *
        *  @return int - Integer containing the corresponding SQLite3 datatype code
        *  Possible results: 1 - INT, 2 - FLOAT, 3 - TEXT, 4 - BLOB, 5 - NULL
        *   0 - ERROR
        */
        const int getType(const int column);

        /**
         *  Returns the size of the item currently in the column
         *
         *  @param column - Integer representing column whose type to check
         *
         *  @return - Integer representing the size of the value currently in the
         *      the column, in bytes.
         */
        const int getSize(const int column);

        /**
         *  Returns a pointer to a null-terminated string in the specified column
         *  of the result of the latest step
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant text from
         *  @return const char* - pointer to the null terminated character array
         *      containing resultant text for a given column or nullptr if column
         *      contains null
         */
        std::string getString(const int column);

        /**
         *  Returns an integer in the specified column of the result of the
         *  latest step
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *  @return int - integer value
         */
        int getInt(const int column);

        /**
         *  Returns an SQLite3 int64 in the specified column of the result of the
         *  latest step
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *  @return int - sqlite3_int64 value
         */
        sqlite3_int64 getInt64(const int column);

        /**
         *  Returns a double precision float in the specified column of the
         *  result of the latest step
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *  @return double - double value
         */
        double getDouble(const int column);

        /**
         *  Returns a blob in the specified column of the
         *  result of the latest step
         *
         *  *RESULTS ARE UNDEFINED IF A ROW WAS NOT RETURNED OR COLUMN IS INVALID*
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant int from
         *  @return - pointer to blob
         */
        const void *getBlob(const int column);

        /**
         *  Returns a ValueHandler objects that functions as a wrapper and
         *  type converter for the range of return types that SQLite supports.
         *  Useful if you want to be able to implicitly cast from a column value
         *
         *  Useage:     int var = stmt.getColumn(1);
         *              double var2 = stmt.getColumn(2);
         *
         *  @param column - Integer representing the column number to pull the
         *      resultant column value from
         *
         *  @return - ValueHandler that wraps an SQLite3 value
         */
        ValueHandler getColumn(const int column);

        /**
         *  Steps the prepared statement a single time
         *
         *  @return bool - Returns true if there are results from the SQL
         *      statement as a result of stepping. Further calls to step or reset
         *      will cause a loss of these results if they are not read using one
         *      of the column readers.
         */
        bool step();

        /**
         *  Resets the prepared statement so it is ready to executed again
         */
        void reset();

        /**
         *  Clears the bindings of a prepared statement (sets them to null)
         *  Not necissary but useful if some fields can/should be null.
         *  The SQLite documentation made special mention of the fact that to reset
         *  and to clear bindings are different functions in their entirety.
         */
        void clear();
    };
}