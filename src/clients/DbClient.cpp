#include "DbClient.h"

#include "ocpn_plugin.h"
#include <wx/filename.h>

namespace MarineNavi {

std::shared_ptr<SQLite::Database> CreateDatabase(std::string dbName) {
  try {
    wxString sep = wxFileName::GetPathSeparator();
    wxString dbPath =
        *GetpPrivateApplicationDataLocation() + sep + "plugins" + sep + dbName;

    auto db = std::make_shared<SQLite::Database>(
        dbPath, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    SQLite::Transaction trans(*db);
    db->exec(
        "CREATE TABLE IF NOT EXISTS forecasts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP NOT NULL, "
        "source INTEGER NOT NULL)");
    db->exec(
        "CREATE TABLE IF NOT EXISTS forecast_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP NOT NULL, "
        "started_at TEXT NOT NULL, "
        "end_at TEXT NOT NULL, "
        "lat REAL NOT NULL, "
        "lon REAL NOT NULL, "
        "wave_height REAL, "
        "swell_height REAL, "
        "forecast_id INTEGER, "
        "FOREIGN KEY(forecast_id) REFERENCES forecasts(id))");
    db->exec(
        "CREATE UNIQUE INDEX IF NOT EXISTS forecast_records_idx ON "
        "forecast_records ("
        "date, lat, lon, forecast_id)");
    trans.commit();
    return db;
  } catch (std::exception& ex) {
    fprintf(stderr, "Failed to create datbase with reason: %s\n", ex.what());
    throw ex;
  } catch (...) {
    fprintf(stderr, "Failed to create datbase with unknown reason");
    throw;
  }
}

}  // namespace MarineNavi
