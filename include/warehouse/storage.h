#pragma once
#include "warehouse/models.h"
#include <vector>
#include <string>
#include <optional>
#include <map>

struct Stats {
  int totalItems;
  int totalQuantity;
  int lowStockItems;
  std::map<std::string, int> itemsByCategory;
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

private:
  Db &db_;
};
