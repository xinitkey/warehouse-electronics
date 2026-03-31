#pragma once
#include <sqlite3.h>
#include <string>

class Db {
public:
  explicit Db(const std::string &filename);
  ~Db();

  void initSchema();

  sqlite3 *handle() { return db_; }
  sqlite3 *handle() const { return db_; }

private:
  sqlite3 *db_ = nullptr;
};
