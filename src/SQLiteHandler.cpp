/**
*  SQLiteHandler.h
*  Provides a portable SQLite3 interface to be used with the StatementHandler
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

#include "SQLiteHandler.h"

namespace SQLiter {

    SQLiteHandler::SQLiteHandler(const std::string location) {
        forceOpenDatabase(location);
    }

    SQLiteHandler::~SQLiteHandler() {
        destroyStatements();
        db.reset();
    }

    void SQLiteHandler::createDatabase(const std::string location) {
       if (!fileExists(location)) {
            sqlite3 *connection = nullptr;
            result(sqlite3_open(location.c_str(), &connection));
            db.reset(connection);
        } else {
            throw SQLiteException("File Already Exists");
        }
    }

    void SQLiteHandler::createDatabase() {
        sqlite3 *connection = nullptr;
        result(sqlite3_open(nullptr, &connection));
        db.reset(connection);
    }

    void SQLiteHandler::openDatabase(const std::string location) {
        if (fileExists(location)) {
            sqlite3 *connection = nullptr;
            result(sqlite3_open(location.c_str(), &connection));
            db.reset(connection);
        } else {
            throw SQLiteException("File Does Not Exist");
        }
    }

    void SQLiteHandler::closeDatabase() {
        destroyStatements();
        db.reset();
    }

    void SQLiteHandler::forceOpenDatabase(const std::string location) {
        sqlite3 *connection = nullptr;
        result(sqlite3_open(location.c_str(), &connection));
        db.reset(connection);
    }

    void SQLiteHandler::load(const std::string location) {
        sqlite3 *connection = nullptr;
        sqlite3_open(location.c_str(), &connection);
        sqlite3_backup *backup = sqlite3_backup_init(db.get(), "main", connection, "main");
        if (backup) {
            sqlite3_backup_step(backup, -1);
            sqlite3_backup_finish(backup);
        }
        sqlite3_close(connection);
    }

    void SQLiteHandler::save(const std::string location) {
        sqlite3 *connection = nullptr;
        sqlite3_open(location.c_str(), &connection);
        sqlite3_backup *backup = sqlite3_backup_init(connection, "main", db.get(), "main");
        if (backup) {
            sqlite3_backup_step(backup, -1);
            sqlite3_backup_finish(backup);
        }
        sqlite3_close(connection);
    }

    StatementHandler *SQLiteHandler::prepareStatement(const std::string key, const std::string stmtStr) {
        stmts.insert(std::make_pair(key, std::unique_ptr<StatementHandler>(new StatementHandler(db.get(), stmtStr))));
        return getStatement(key);
    }

    StatementHandler *SQLiteHandler::getStatement(const std::string key) {
        return stmts.at(key).get();
    }

    void SQLiteHandler::destroyStatement(const std::string key) {
        stmts.erase(key);
    }

    void SQLiteHandler::destroyStatements() {
        stmts.clear();
    }

    int SQLiteHandler::rawExec(const std::string stmtStr) {
        result(sqlite3_exec(db.get(), stmtStr.c_str(), NULL, NULL, NULL));
        return changes();
    }

    void SQLiteHandler::result(const int resCode) {
        if (resCode != SQLITE_OK)
            throw SQLiteException(sqlite3_errmsg(db.get()));
    }

    void SQLiteHandler::scalarFunction(const std::string name, int nArg, void *pApp,
        void(*xFunc)(sqlite3_context*, int, sqlite3_value**),
        void(*xDestroy)(void*)) {
        sqlite3_create_function_v2(db.get(), name.c_str(), nArg, SQLITE_UTF8, pApp, xFunc, NULL, NULL, xDestroy);
    }

    void SQLiteHandler::aggregateFunction(const std::string name, int nArg, void *pApp,
        void(*xStep)(sqlite3_context*, int, sqlite3_value**),
        void(*xFinal)(sqlite3_context*),
        void(*xDestroy)(void*)) {
        sqlite3_create_function_v2(db.get(), name.c_str(), nArg, SQLITE_UTF8, pApp, NULL, xStep, xFinal, xDestroy);
    }

    void SQLiteHandler::deleteFunction(const std::string name) {
        sqlite3_create_function_v2(db.get(), name.c_str(), NULL, SQLITE_UTF8, NULL, NULL, NULL, NULL, NULL);
    }

    int SQLiteHandler::changes() {
        return sqlite3_changes(db.get());
    }

    int SQLiteHandler::totalChanges() {
        return sqlite3_total_changes(db.get());
    }

    int SQLiteHandler::errorCode() {
        return sqlite3_errcode(db.get());
    }

    const std::string SQLiteHandler::errorMsg() {
        return sqlite3_errmsg(db.get());
    }
}