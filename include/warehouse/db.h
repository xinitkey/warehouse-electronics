#pragma once
#include <string>
#include <sqlite3.h>

class Db {
public:
    explicit Db(const std::string& path);
    ~Db();

    sqlite3* handle() const { return db_; }
    void initSchema();

private:
    sqlite3* db_ = nullptr;
};
