/**
 *  SQLiteException.h
 *  Simple exception object to be used by the rest of the library.
 *
 *  @author William Horstkamp
 *  @version 0.5
 */

#include <stdexcept>

#ifndef SQLITEEXCEPTION_H
#define SQLITEEXCEPTION_H

/**
 *  Simple exception class wraps an error message, typically one derived from
 *  an SQLite3 result code, but also others such as file i/o exceptions thrown
 *  by database create and open functions.
 */

class SQLiteException : public std::runtime_error {
public:
    /**
     *  Constructor accepts a pointer to a C string containing the error
     *  message. Hands that error message to it's parent constructor.
     *
     *  @param errMsg - Null terminated C String containing the error message
     *      to built an SQLiteException around.
     *
     *  @return - SQLiteException wrapping the error message to be thrown.
     */
    SQLiteException(const char *errMsg) : std::runtime_error(errMsg) {};
};

#endif