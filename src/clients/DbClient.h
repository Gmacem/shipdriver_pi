#pragma once

#include "ForecastsProvider.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <memory>
#include <string>

namespace MarineNavi {

class DbClient {
public:
  DbClient(std::shared_ptr<SQLite::Database> db) : db_(db) {}

  void AddForecast(time_t started_at, time_t finished_at,
                   int source) {  // TODO: batch upsert
  }

private:
  std::shared_ptr<SQLite::Database> db_;
};

std::shared_ptr<SQLite::Database> CreateDatabase(std::string dbName);

}  // namespace MarineNavi
