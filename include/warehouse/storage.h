#pragma once
#include <vector>
#include "warehouse/models.h"

class Db;

class Storage {
public:
    explicit Storage(Db& db);

    std::vector<Item> loadAll();
    void addItem(const Item& item);

private:
    Db& db_;
};
