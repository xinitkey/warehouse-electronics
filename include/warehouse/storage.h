#pragma once
#include "warehouse/models.h"
#include <vector>
#include <string>
#include <optional>
#include <map>
#include <chrono>

struct Stats {
  int totalItems;
  int totalQuantity;
  int lowStockItems;
  double totalValue;
  std::map<std::string, int> itemsByCategory;
};

enum class OperationType {
  Purchase,   // Приход
  Sale,       // Расход
  Return,     // Возврат
  WriteOff    // Списание
};

struct OperationLog {
  int id;
  int itemId;
  std::string itemName;
  OperationType type;
  int quantityChange;
  std::string comment;
  std::chrono::system_clock::time_point timestamp;
};

class Db;

class Storage {
public:
  explicit Storage(Db &db);

  std::vector<Item> loadAll();
  std::vector<Item> search(const std::string &query);
  void addItem(const Item &item);
  bool loadById(int id, Item &out);
  void updateItem(const Item &item);
  void deleteItem(int id);
  Stats getStats(int minQuantityThreshold = 5);
  
  // Operation log
  void logOperation(int itemId, const std::string &itemName, OperationType type, 
                    int quantityChange, const std::string &comment = "");
  std::vector<OperationLog> getOperationLog(int itemId = -1, int limit = 50);
  
  // CSV Import/Export
  void exportToCsv(const std::string &filename);
  void importFromCsv(const std::string &filename);

private:
  Db &db_;
};
