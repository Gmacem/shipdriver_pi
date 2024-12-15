#pragma once

#include <memory>

class wxWindow;

namespace SQLite {
class Database;
}

namespace marine_navi {

class CheckPathCase;
class RenderOverlay;
class DbClient;
class ForecastsLoader;

struct Dependencies {
  std::shared_ptr<marine_navi::CheckPathCase> CheckPathCase;
  std::shared_ptr<marine_navi::ForecastsLoader> ForecastsLoader;
  std::shared_ptr<marine_navi::RenderOverlay> RenderOverlay;
  std::shared_ptr<SQLite::Database> Db;
  std::shared_ptr<marine_navi::DbClient> DbClient;
  wxWindow* OcpnCanvasWindow;
};

Dependencies CreateDependencies(wxWindow* OcpnCanvasWindow);

}  // namespace marine_navi