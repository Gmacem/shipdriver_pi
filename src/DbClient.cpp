#include "DbClient.h"

#include "ocpn_plugin.h"
#include <wx/filename.h>

namespace MarineNavi {

std::shared_ptr<SQLite::Database> CreateDatabase(std::string dbName) {
    wxString dbPath = *GetpPrivateApplicationDataLocation()
        + wxFileName::GetPathSeparator() + dbName;
    
    bool haveToCreate = false;
    if (!wxFileExists(dbPath)) {
        haveToCreate = true;
    }
    auto db = std::make_shared<SQLite::Database>(dbName, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    if (haveToCreate) {
        SQLite::Transaction trans(*db);
        db->exec(
            "CREATE TABLE IF NOT EXISTS forecasts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "created_at TEXT DEFAULT CURRENT_TIMESTAMP NOT NULL, "
            "from TEXT NOT NULL, "
            "source INTEGER NOT NULL)"
        );
        db->exec(
            "CREATE TABLE IF NOT EXISTS forecast_records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "created_at TEXT DEFAULT CURRENT_TIMESTAMP NOT NULL, "
            "date TEXT NOT NULL, "
            "lat REAL NOT NULL, "
            "lon REAL NOT NULL, "
            "wave_height REAL, "
            "swell_height REAL, "
            "forecast_id INTEGER, "
            "FOREIGN KEY(forecast_id) REFERENCES forecasts(id))"
        );
        trans.commit();
    }
    return db;
}

} // namespace MarineNavi
