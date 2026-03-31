#include "warehouse/storage.h"
#include "warehouse/db.h"
#include <sqlite3.h>
#include <stdexcept>

Storage::Storage(Db &db) : db_(db) {}

std::vector<Item> Storage::loadAll() {
  const char *sql =
      "SELECT id, name, category, location, quantity FROM items ORDER BY id;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare SELECT");
  }

  std::vector<Item> items;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Item it;
    it.id = sqlite3_column_int(stmt, 0);
    it.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    it.category = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    it.location = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    it.quantity = sqlite3_column_int(stmt, 4);
    items.push_back(std::move(it));
  }

  sqlite3_finalize(stmt);
  return items;
}

std::vector<Item> Storage::search(const std::string &query) {
  const char *sql =
      "SELECT id, name, category, location, quantity FROM items "
      "WHERE name LIKE ? OR category LIKE ? OR location LIKE ? "
      "ORDER BY id;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare search query");
  }

  std::string pattern = "%" + query + "%";
  sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);

  std::vector<Item> items;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Item it;
    it.id = sqlite3_column_int(stmt, 0);
    it.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    it.category = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    it.location = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    it.quantity = sqlite3_column_int(stmt, 4);
    items.push_back(std::move(it));
  }

  sqlite3_finalize(stmt);
  return items;
}

void Storage::addItem(const Item &item) {
  const char *sql = "INSERT INTO items(name, category, location, quantity) "
                    "VALUES(?, ?, ?, ?);";

  sqlite3_stmt *stmt = nullptr;
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

bool Storage::loadById(int id, Item &out) {
  sqlite3_stmt *stmt = nullptr;
  const char *sql = "SELECT id, name, category, location, quantity "
                    "FROM items WHERE id = ?";

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare loadById");

  sqlite3_bind_int(stmt, 1, id);

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    out.id = sqlite3_column_int(stmt, 0);
    out.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    out.category = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    out.location = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    out.quantity = sqlite3_column_int(stmt, 4);
    sqlite3_finalize(stmt);
    return true;
  }

  sqlite3_finalize(stmt);
  return false;
}

void Storage::updateItem(const Item &item) {
  sqlite3_stmt *stmt = nullptr;
  const char *sql = "UPDATE items "
                    "SET name = ?, category = ?, location = ?, quantity = ? "
                    "WHERE id = ?";

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare updateItem");

  sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, item.category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, item.location.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, item.quantity);
  sqlite3_bind_int(stmt, 5, item.id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::runtime_error("Failed to execute updateItem");
  }

  sqlite3_finalize(stmt);
}

void Storage::deleteItem(int id) {
  sqlite3_stmt *stmt = nullptr;
  const char *sql = "DELETE FROM items WHERE id = ?";

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare deleteItem");

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::runtime_error("Failed to execute deleteItem");
  }

  sqlite3_finalize(stmt);
}

Stats Storage::getStats(int minQuantityThreshold) {
  Stats stats{0, 0, 0, {}};

  auto items = loadAll();
  stats.totalItems = static_cast<int>(items.size());

  for (const auto &item : items) {
    stats.totalQuantity += item.quantity;
    if (item.quantity <= minQuantityThreshold) {
      stats.lowStockItems++;
    }
    if (!item.category.empty()) {
      stats.itemsByCategory[item.category]++;
    }
  }

  return stats;
}
