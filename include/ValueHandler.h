/**
 *  ValueHandler.h
 *  Provides a wrapper for an SQLite3 prepared statement's return types.
 *  Useful predominantly to pretty up code or take advantage of SQLite3's
 *  dynamic typing.
 *
 *  @author William Horstkamp
 *  @version 0.5
 */

#include <sqlite3.h>

namespace SQLiter {

    /**
     *  Wrapper for the various types of values that can be stored in an SQlite3
     *  field. Used as the return type of StatementHandler.getColumn(const int).
     *  Class consists of a series of inline functions used for implicit type
     *  coversion in the form of varType var = StatementHandler.getColumn(const int)
     *
     *  **CASTING TO THE WRONG TYPE IS UNDEFINED AND OFTEN UNSAFE - BE WARNED**
     *  **CHECK THE COL TYPE WITH StatementHandler.getType(const int) BEFOREHAND**
     */
    class ValueHandler {
    private:
        sqlite3_stmt *stmt;
        int colNum;
    public:

        /**
         *  Constructor accepts pointer to statement and column number to build a
         *  representation of the values currently stored in that field.
         *
         *  @param inputStmt - Pointer to the statement that contains the value
         *  @param inputColNum - Integer representing the column that contains
         *      the value.
         *
         *  @return - ValueHandler object that wraps the value.
         */
        ValueHandler(sqlite3_stmt *inputStmt, const int inputColNum) :
            stmt(inputStmt), colNum(inputColNum) {};

        /**
        *   Integer conversion operator for implicit type coversion to integer
        *
        *   @return - Integer value contained within the given column and statement
        */
        inline operator int() const  {
            return sqlite3_column_int(stmt, colNum);
        }

        /**
        *   Int64 conversion operator for implicit type coversion to int64
        *
        *   @return - sqlite_int64 value contained within the given column and
        *       statement
        */
        inline operator sqlite3_int64() const {
            return sqlite3_column_int64(stmt, colNum);
        }

        /**
        *  Blob conversion operator for implicit type coversion to blob
        *
        *   @return - Pointer to sqlite3 blob
        */
        inline operator const void *() const  {
            return sqlite3_column_blob(stmt, colNum);
        }

        /**
        *   Double conversion operator for implicit type coversion to floating
        *   point
        *
        *   @return - Double value contained within the given column and statement
        */
        inline operator double() const {
            return sqlite3_column_double(stmt, colNum);
        }

        /**
        *   String conversion operator for implicit type coversion to C string
        *
        *   @return - Pointer to null-terminated C String
        */
        inline operator const char *() const {
            return (const char*)sqlite3_column_text(stmt, colNum);
        }

    };
}