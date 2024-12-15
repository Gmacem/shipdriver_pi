#include "DbClient.h"

#include "ocpn_plugin.h"
#include <wx/filename.h>

#include <iomanip>
#include <string>

namespace marine_navi {

namespace {
constexpr int BUFFER_SIZE = 1024;

using BaseArgVar = clients::query_builder::BaseArgVar;
using ComposedArgVar = clients::query_builder::ComposedArgVar;

char const* kInsertForecastRecordQuery =
    "INSERT INTO forecast_records ("
    "started_at, end_at, lat, lon, wave_height, swell_height, forecast_id) "
    "VALUES ('%s', '%s', %lf, %lf, %s, %s, %d) "
    "RETURNING id";

char const* kSelectClosestForecasts =
    "WITH path(id, X, Y) AS ("
    "  VALUES %s"
    "), fwd as ("
    "  SELECT forecast_records.id as fid, path.id as pid,"
    "    (forecast_records.lon - path.X) * (forecast_records.lon - path.X) "
    "      + (forecast_records.lat - path.Y) * (forecast_records.lat - path.Y) "
    "      as dst, forecast_records.wave_height, forecast_records.swell_height "
    "  from forecast_records "
    "  CROSS JOIN path "
    "  WHERE '%s' < forecast_records.end_at) "
    "SELECT mfwd.dst, fwd.dst, fwd.fid, fwd.pid, fwd.wave_height, "
    "fwd.swell_height from fwd "
    "INNER JOIN ( "
    "  select fid, min(dst) as dst FROM fwd GROUP BY pid) mfwd "
    "on fwd.dst = mfwd.dst and fwd.fid = mfwd.fid";

}  // namespace

int64_t DbClient::InsertForecast(ForecastsSource source) {
  const std::string kQueryName = "kInsertForecast";
  const auto& template = query_storage_->GetTemplate(kQueryName);
  const auto query = template.MakeQuery({static_cast<int64_t>(source)});

  return InsertQuery(query);
}

void DbClient::InsertForecastRecordBatch(const std::vector<ForecastRecord>& records,
                                         int64_t forecastId) {
  static constexpr int BATCH_SIZE = 512;
  const std::string kQueryName = "kInserForecastRecordBatchQuery";
  const auto& template = query_storage_->GetTemplate(kQueryName);

  SQLite::Transaction trans(*db_);
  for (size_t i = 0; i < records.size(); i += BATCH_SIZE) {
    size_t bound = std::min(i + BATCH_SIZE, records.size());
    std::vector<ComposedArgVar> outer;
    for (size_t j = i; j < bound; ++j) {
      std::vector<std::vector<BaseArgVar>> inner{
        records[j].started_at,
        records[j].end_at,
        records[j].lat,
        records[j].lon,
        records[j].wave_height,
        records[j].swell_height,
        forecastId
      };
      outer.emplace_back(std::move(inner));
    }
    db_->exec(template.MakeQuery(args));
    wxLogInfo(wxT("Load progress: %lu/%lu"), bound, records.size());
  }
  trans.commit();
}

int64_t DbClient::InsertQuery(std::string query) {
  try {
    auto result = db_->execAndGet(query);
    return result.getInt();
  } catch (std::exception& ex) {
    fprintf(stderr, "Failed to exec query %s with error: %s", query.data(),
            ex.what());
    throw ex;
  }
}

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
        "forecast_id INTEGER NOT NULL, "
        "FOREIGN KEY(forecast_id) REFERENCES forecasts(id))");
    db->exec(
        "CREATE UNIQUE INDEX IF NOT EXISTS forecast_records_idx ON "
        "forecast_records ("
        "started_at, end_at, lat, lon, forecast_id)");
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

std::vector<std::tuple<int, std::optional<double>, std::optional<double>>>
DbClient::SelectNearestForecasts(
    const std::vector<std::pair<int, marine_navi::Utils::Point>>& points_with_id,
    const std::string& date) {

  const std::string kQueryName = "kSelectClosestForecasts";
  const auto& template = query_storage_->GetTemplate(kQueryName);

  std::vector<std::vector<clients::::BaseArgVar> > first_arg(points_with_id.size());
  for (size_t i = 0; i < points_with_id.size(); ++i) {
    const auto& [id, point] = points_with_id[i];
    first_arg[i] = std::vector<clients::::BaseArgVar>{
      id, point.X(), point.Y()
    };
  }

  auto query = marine_navi::Utils::StringFormat(kSelectClosestForecasts,
                                               ss.str().data(), date.data());
  fprintf(stderr, "Nearest point query: %s\n", query.data());
  SQLite::Statement st(*db_, query);

  std::vector<std::tuple<int, std::optional<double>, std::optional<double>>>
      result;

  while (st.executeStep()) {
    int id = st.getColumn(3).getInt();
    auto wave_column = st.getColumn(4);
    auto swell_columm = st.getColumn(5);

    std::optional<double> wave_height;
    std::optional<double> swell_height;
    if (!wave_column.isNull()) {
      wave_height = wave_column.getDouble();
    }
    if (!swell_columm.isNull()) {
      swell_height = swell_columm.getDouble();
    }
    result.emplace_back(id, wave_height, swell_height);
  }
  return result;
}

}  // namespace marine_navi
