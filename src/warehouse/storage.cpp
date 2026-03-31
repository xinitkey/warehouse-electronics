#include "warehouse/storage.h"
#include "warehouse/db.h"
#include <sqlite3.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>

Storage::Storage(Db &db) : db_(db) {}

std::vector<Item> Storage::loadAll() {
  const char *sql =
      "SELECT id, name, category, location, quantity, sku, price, supplier, min_quantity, description "
      "FROM items ORDER BY id;";
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
    it.sku = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    it.price = sqlite3_column_double(stmt, 6);
    it.supplier = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
    it.minQuantity = sqlite3_column_int(stmt, 8);
    it.description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
    items.push_back(std::move(it));
  }

  sqlite3_finalize(stmt);
  return items;
}

std::vector<Item> Storage::search(const std::string &query) {
  const char *sql =
      "SELECT id, name, category, location, quantity, sku, price, supplier, min_quantity, description "
      "FROM items "
      "WHERE name LIKE ? OR category LIKE ? OR location LIKE ? OR sku LIKE ? OR supplier LIKE ? "
      "ORDER BY id;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare search query");
  }

  std::string pattern = "%" + query + "%";
  sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, pattern.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, pattern.c_str(), -1, SQLITE_TRANSIENT);

  std::vector<Item> items;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Item it;
    it.id = sqlite3_column_int(stmt, 0);
    it.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    it.category = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    it.location = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    it.quantity = sqlite3_column_int(stmt, 4);
    it.sku = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    it.price = sqlite3_column_double(stmt, 6);
    it.supplier = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
    it.minQuantity = sqlite3_column_int(stmt, 8);
    it.description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
    items.push_back(std::move(it));
  }

  sqlite3_finalize(stmt);
  return items;
}

void Storage::addItem(const Item &item) {
  const char *sql = "INSERT INTO items(name, category, location, quantity, sku, price, supplier, min_quantity, description) "
                    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare INSERT");
  }

  sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, item.category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, item.location.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, item.quantity);
  sqlite3_bind_text(stmt, 5, item.sku.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 6, item.price);
  sqlite3_bind_text(stmt, 7, item.supplier.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 8, item.minQuantity);
  sqlite3_bind_text(stmt, 9, item.description.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::runtime_error("Failed to execute INSERT");
  }

  sqlite3_finalize(stmt);
}

bool Storage::loadById(int id, Item &out) {
  sqlite3_stmt *stmt = nullptr;
  const char *sql = "SELECT id, name, category, location, quantity, sku, price, supplier, min_quantity, description "
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
    out.sku = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    out.price = sqlite3_column_double(stmt, 6);
    out.supplier = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
    out.minQuantity = sqlite3_column_int(stmt, 8);
    out.description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
    sqlite3_finalize(stmt);
    return true;
  }

  sqlite3_finalize(stmt);
  return false;
}

void Storage::updateItem(const Item &item) {
  sqlite3_stmt *stmt = nullptr;
  const char *sql = "UPDATE items "
                    "SET name = ?, category = ?, location = ?, quantity = ?, "
                    "sku = ?, price = ?, supplier = ?, min_quantity = ?, description = ? "
                    "WHERE id = ?";

  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK)
    throw std::runtime_error("Failed to prepare updateItem");

  sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, item.category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, item.location.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, item.quantity);
  sqlite3_bind_text(stmt, 5, item.sku.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 6, item.price);
  sqlite3_bind_text(stmt, 7, item.supplier.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 8, item.minQuantity);
  sqlite3_bind_text(stmt, 9, item.description.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 10, item.id);

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
  Stats stats{0, 0, 0, 0.0, {}};

  auto items = loadAll();
  stats.totalItems = static_cast<int>(items.size());

  for (const auto &item : items) {
    stats.totalQuantity += item.quantity;
    stats.totalValue += item.price * item.quantity;
    if (item.quantity <= item.minQuantity) {
      stats.lowStockItems++;
    }
    if (!item.category.empty()) {
      stats.itemsByCategory[item.category]++;
    }
  }

  return stats;
}

void Storage::logOperation(int itemId, const std::string &itemName, OperationType type,
                           int quantityChange, const std::string &comment) {
  const char *sql = "INSERT INTO operation_logs(item_id, item_name, operation_type, quantity_change, comment) "
                    "VALUES(?, ?, ?, ?, ?);";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_.handle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare logOperation");
  }

  sqlite3_bind_int(stmt, 1, itemId);
  sqlite3_bind_text(stmt, 2, itemName.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, static_cast<int>(type));
  sqlite3_bind_int(stmt, 4, quantityChange);
  sqlite3_bind_text(stmt, 5, comment.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw std::runtime_error("Failed to execute logOperation");
  }

  sqlite3_finalize(stmt);
}

std::vector<OperationLog> Storage::getOperationLog(int itemId, int limit) {
  std::string sql = "SELECT id, item_id, item_name, operation_type, quantity_change, comment, timestamp "
                    "FROM operation_logs ";
  if (itemId >= 0) {
    sql += "WHERE item_id = ? ";
  }
  sql += "ORDER BY timestamp DESC LIMIT ?;";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_.handle(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("Failed to prepare getOperationLog");
  }

  int param = 1;
  if (itemId >= 0) {
    sqlite3_bind_int(stmt, param++, itemId);
  }
  sqlite3_bind_int(stmt, param, limit);

  std::vector<OperationLog> logs;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    OperationLog log;
    log.id = sqlite3_column_int(stmt, 0);
    log.itemId = sqlite3_column_int(stmt, 1);
    log.itemName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    log.type = static_cast<OperationType>(sqlite3_column_int(stmt, 3));
    log.quantityChange = sqlite3_column_int(stmt, 4);
    log.comment = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    // timestamp handled separately if needed
    logs.push_back(log);
  }

  sqlite3_finalize(stmt);
  return logs;
}

void Storage::exportToCsv(const std::string &filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for export: " + filename);
  }

  // Header
  file << "ID,Name,Category,Location,Quantity,SKU,Price,Supplier,MinQuantity,Description\n";

  auto items = loadAll();
  for (const auto &item : items) {
    file << item.id << ","
         << "\"" << item.name << "\","
         << "\"" << item.category << "\","
         << "\"" << item.location << "\","
         << item.quantity << ","
         << "\"" << item.sku << "\","
         << std::fixed << std::setprecision(2) << item.price << ","
         << "\"" << item.supplier << "\","
         << item.minQuantity << ","
         << "\"" << item.description << "\"\n";
  }
}

void Storage::importFromCsv(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for import: " + filename);
  }

  std::string line;
  bool isHeader = true;

  while (std::getline(file, line)) {
    if (isHeader) {
      isHeader = false;
      continue;
    }

    // Simple CSV parsing (doesn't handle escaped quotes perfectly)
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
      // Remove quotes
      if (!token.empty() && token.front() == '"') {
        token = token.substr(1);
      }
      if (!token.empty() && token.back() == '"') {
        token.pop_back();
      }
      tokens.push_back(token);
    }

    if (tokens.size() >= 5) {
      Item item;
      item.id = 0;
      item.name = tokens[1];
      item.category = tokens[2];
      item.location = tokens[3];
      item.quantity = std::stoi(tokens[4]);
      item.sku = tokens.size() > 5 ? tokens[5] : "";
      item.price = tokens.size() > 6 ? std::stod(tokens[6]) : 0.0;
      item.supplier = tokens.size() > 7 ? tokens[7] : "";
      item.minQuantity = tokens.size() > 8 ? std::stoi(tokens[8]) : 5;
      item.description = tokens.size() > 9 ? tokens[9] : "";

      addItem(item);
    }
  }
}
