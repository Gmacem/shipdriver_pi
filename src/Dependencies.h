#pragma once

#include <memory>

class wxWindow;

namespace SQLite {
class Database;
}

namespace MarineNavi {

class CheckPathCase;
class RenderOverlay;
class DbClient;
class ForecastsLoader;

struct Dependencies {
  std::shared_ptr<MarineNavi::CheckPathCase> CheckPathCase;
  std::shared_ptr<MarineNavi::ForecastsLoader> ForecastsLoader;
  std::shared_ptr<MarineNavi::RenderOverlay> RenderOverlay;
  std::shared_ptr<SQLite::Database> Db;
  std::shared_ptr<MarineNavi::DbClient> DbClient;
  wxWindow* OcpnCanvasWindow;
};

Dependencies CreateDependencies(wxWindow* OcpnCanvasWindow);

}  // namespace MarineNavi