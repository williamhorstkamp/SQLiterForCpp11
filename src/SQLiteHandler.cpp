/**
*  SQLiteHandler.h
*  Provides a portable SQLite3 interface to be used with the StatementHandler
*
*  @author William Horstkamp
*  @version 0.6
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

#include "SQLiteHandler.h"

SQLiteHandler::SQLiteHandler(const char *location) {
   forceOpenDatabase(location);
}

SQLiteHandler::~SQLiteHandler() {
    destroyStatements();
    db.reset();
}

StatementHandler *SQLiteHandler::prepareStatement(const char *key, const char *stmtStr) {
    stmts.insert(std::make_pair(key, std::unique_ptr<StatementHandler>(new StatementHandler(db.get(), stmtStr))));
    return getStatement(key);
}


void SQLiteHandler::destroyStatements() {
    stmts.clear();
}

StatementHandler *SQLiteHandler::getStatement(const char *key) {
    return stmts.at(key).get();
}

void SQLiteHandler::forceOpenDatabase(const char *location) {
    sqlite3 *connection = nullptr;
    result(sqlite3_open(location, &connection));
    db.reset(connection);
}

void SQLiteHandler::openDatabase(const char *location) {
    if (fileExists(location)) {
        sqlite3 *connection = nullptr;
        result(sqlite3_open(location, &connection));
        db.reset(connection);
    } else {
        throw SQLiteException("File Does Not Exist");
    }
}

void SQLiteHandler::createDatabase(const char *location) {
    if (!fileExists(location)) {
        sqlite3 *connection = nullptr;
        result(sqlite3_open(location, &connection));
        db.reset(connection);
    } else {
        throw SQLiteException("File Already Exists");
    }
}

void SQLiteHandler::closeDatabase() {
    destroyStatements();
    db.reset();
}

int SQLiteHandler::rawExec(const char *stmtStr) {
    result(sqlite3_exec(db.get(), stmtStr, NULL, NULL, NULL));
    return sqlite3_changes(db.get());
}

void SQLiteHandler::result(const int resCode) {
    if (resCode != SQLITE_OK)
        throw SQLiteException(sqlite3_errmsg(db.get()));
}

void SQLiteHandler::scalarFunction(const char *name, int nArg, void *pApp,
    void(*xFunc)(sqlite3_context*, int, sqlite3_value**),
    void(*xDestroy)(void*)) {
    sqlite3_create_function_v2(db.get(), name, nArg, SQLITE_UTF8, pApp, xFunc, NULL, NULL, xDestroy);
}

void SQLiteHandler::aggregateFunction(const char *name, int nArg, void *pApp,
    void(*xStep)(sqlite3_context*, int, sqlite3_value**),
    void(*xFinal)(sqlite3_context*),
    void(*xDestroy)(void*)) {
    sqlite3_create_function_v2(db.get(), name, nArg, SQLITE_UTF8, pApp, NULL, xStep, xFinal, xDestroy);
}

void SQLiteHandler::deleteFunction(const char*name) {
    sqlite3_create_function_v2(db.get(), name, NULL, SQLITE_UTF8, NULL, NULL, NULL, NULL, NULL);
}