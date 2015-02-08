/**
 *  StatementHandler.cpp
 *  Provides a wrapper for a unique_ptr<sqlite3_stmt> that both
 *  cleanly prepares the statement and manages the lifetime of it
 *  with the bonus of providing a convenient interface
 *
 *  @author William Horstkamp
 *  @version 0.8
 */

/**
 *  SQLiter For C++11 is an SQLite3 wrapper with C++11 features.
 *  Copyright (C) 2015 William Horstkamp
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *	DEALINGS IN THE SOFTWARE.
 */

#include <cstring>
#include "SQLiteException.h"
#include "StatementHandler.h"

namespace SQLiter {

    StatementHandler::StatementHandler(sqlite3 *db, const std::string stmtStr) {
        sqlite3_stmt *prepStmt;
        sqlite3_prepare_v2(db, stmtStr.c_str(), strlen(stmtStr.c_str()), &prepStmt, nullptr);
        stmt = std::unique_ptr<sqlite3_stmt, Closesqlite3_stmt>(prepStmt);
    }

    StatementHandler::~StatementHandler() {
        inputAlias.clear();
        outputAlias.clear();
        stmt.reset();
    }

    void StatementHandler::bind(const int var, const std::string input) {
        sqlite3_bind_text(stmt.get(), var, input.c_str(), strlen(input.c_str()), SQLITE_TRANSIENT);
    }

    void StatementHandler::bind(const int var, const int input) {
        sqlite3_bind_int(stmt.get(), var, input);
    }

    void StatementHandler::bind(const int var, const double input) {
        sqlite3_bind_double(stmt.get(), var, input);
    }

    void StatementHandler::bind(const int var, const void *input, const int size) {
        sqlite3_bind_blob(stmt.get(), var, input, size, SQLITE_TRANSIENT);
    }

    void StatementHandler::bindNull(const int var) {
        sqlite3_bind_null(stmt.get(), var);
    }

    const int StatementHandler::getType(const int column) {
        int typeNum = sqlite3_column_type(stmt.get(), column);
        return (typeNum >= SQLITE_INTEGER && typeNum <= SQLITE_NULL ? typeNum : 0);
    }

    const int StatementHandler::getSize(const int column) {
        return sqlite3_column_bytes(stmt.get(), column);
    }

    const std::string StatementHandler::getString(const int column) {
        if (getType(column) == SQLITE_TEXT) {
            return std::string((const char*)sqlite3_column_text(stmt.get(), column));
        } throw SQLiteException("Column doesn't contain a string");
    }

    const int StatementHandler::getInt(const int column) {
        if (getType(column) == SQLITE_INTEGER) {
            return sqlite3_column_int(stmt.get(), column);
        } throw SQLiteException("Column doesn't contain a int");
    }

    const sqlite3_int64 StatementHandler::getInt64(const int column) {
        if (getType(column) == SQLITE_INTEGER) {
            return sqlite3_column_int64(stmt.get(), column);
        } throw SQLiteException("Column doesn't contain a int");
    }

    const double StatementHandler::getDouble(const int column) {
        if (getType(column) == SQLITE_FLOAT) {
            return sqlite3_column_double(stmt.get(), column);
        } throw SQLiteException("Column doesn't contain a float");
    }

    const void *StatementHandler::getBlob(const int column) {
        if (getType(column) == SQLITE_BLOB) {
            return sqlite3_column_blob(stmt.get(), column);
        } throw SQLiteException("Column doesn't contain a blob");
    }

    const ValueHandler StatementHandler::getColumn(const int column) {
        return ValueHandler(stmt.get(), column);
    }

    const bool StatementHandler::step() {
        return (sqlite3_step(stmt.get()) == SQLITE_ROW);
    }

    void StatementHandler::reset() {
        sqlite3_reset(stmt.get());
    }

    void StatementHandler::clear() {
        sqlite3_clear_bindings(stmt.get());
    }

    const int StatementHandler::columnCount() {
        return sqlite3_column_count(stmt.get());
    }

    const std::string StatementHandler::databaseName(const int col) {
        return sqlite3_column_database_name(stmt.get(), col);
    }

    const std::string StatementHandler::tableName(const int col) {
        return sqlite3_column_table_name(stmt.get(), col);
    }

    const std::string StatementHandler::columnName(const int col) {
        return sqlite3_column_origin_name(stmt.get(), col);
    }

    void StatementHandler::setInputAlias(const std::string alias, const int colNum) {
        inputAlias.insert(std::pair<std::string, int>(alias, colNum));
    }

    void StatementHandler::setOutputAlias(const std::string alias, const int colNum) {
        outputAlias.insert(std::pair<std::string, int>(alias, colNum));
    }
}