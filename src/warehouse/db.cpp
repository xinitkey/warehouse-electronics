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

void Db::migrateSchema() {
    const char* migrations[] = {
        // Add extended fields to items
        "ALTER TABLE items ADD COLUMN sku TEXT DEFAULT '';",
        "ALTER TABLE items ADD COLUMN price REAL DEFAULT 0.0;",
        "ALTER TABLE items ADD COLUMN supplier TEXT DEFAULT '';",
        "ALTER TABLE items ADD COLUMN min_quantity INTEGER DEFAULT 5;",
        "ALTER TABLE items ADD COLUMN description TEXT DEFAULT '';",
        
        // Create operation_logs table
        "CREATE TABLE IF NOT EXISTS operation_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "item_id INTEGER NOT NULL,"
        "item_name TEXT NOT NULL,"
        "operation_type INTEGER NOT NULL,"
        "quantity_change INTEGER NOT NULL,"
        "comment TEXT DEFAULT '',"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(item_id) REFERENCES items(id)"
        ");",
        
        // Create index for faster lookups
        "CREATE INDEX IF NOT EXISTS idx_operation_logs_item_id ON operation_logs(item_id);"
    };

    char* err = nullptr;
    for (const auto& migration : migrations) {
        if (sqlite3_exec(db_, migration, nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "unknown error";
            sqlite3_free(err);
            // Ignore "duplicate column" errors during migration
            if (msg.find("duplicate column") == std::string::npos) {
                throw std::runtime_error("DB migration error: " + msg);
            }
        }
    }
}
