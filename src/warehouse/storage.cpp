#include "warehouse/storage.h"
#include "warehouse/db.h"
#include <sqlite3.h>
#include <stdexcept>

Storage::Storage(Db& db) : db_(db) {}

std::vector<Item> Storage::loadAll() {
    const char* sql = "SELECT id, name, category, location, quantity FROM items ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare SELECT");
    }

    std::vector<Item> items;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Item it;
        it.id       = sqlite3_column_int(stmt, 0);
        it.name     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        it.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        it.location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        it.quantity = sqlite3_column_int(stmt, 4);
        items.push_back(std::move(it));
    }

    sqlite3_finalize(stmt);
    return items;
}

void Storage::addItem(const Item& item) {
    const char* sql =
        "INSERT INTO items(name, category, location, quantity) "
        "VALUES(?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare INSERT");
    }

    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, item.location.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, item.quantity);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute INSERT");
    }

    sqlite3_finalize(stmt);
}
