#include "warehouse/db.h"
#include <stdexcept>

Db::Db(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open DB");
    }
}

Db::~Db() {
    if (db_) sqlite3_close(db_);
}

void Db::initSchema() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS items ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "category TEXT,"
        "location TEXT,"
        "quantity INTEGER NOT NULL DEFAULT 0"
        ");";

    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown error";
        sqlite3_free(err);
        throw std::runtime_error("DB schema error: " + msg);
    }
}
