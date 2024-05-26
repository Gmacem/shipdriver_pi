#include "Esimo.h"

#include "common/Utils.h"

#include <chrono>
#include <fstream>

namespace MarineNavi {
namespace {
std::string BuildUrl(std::string resourceId,
                     std::optional<std::string> filter) {
  std::string url =
      "http://esimo.ru/dataview/getresourceexport?format=csv&resourceid=" +
      resourceId;
  if (filter.has_value()) {
    url += "&filter=" + *filter;
  }

  return url;
}

int PushRow(void* params, int column_cnt, const char** column_values) {
  auto* records = static_cast<std::vector<ForecastRecord>*>(params);
  if (column_cnt < 12) {
    fprintf(stderr, "Failed to parse esimo records");
    return 1;
  }
  if (column_values[0] == std::string("Дата и время: начало(минимум)")) {
    return 0;
  }
  try {
    records->push_back(ForecastRecord{
        .StartedAt = Utils::ParseDate(column_values[0]),
        .Date = Utils::ParseDate(column_values[1]),
        .Lat = std::stod(column_values[2]),
        .Lon = std::stod(column_values[3]),
        .WaveHeight = std::stod(column_values[6]),
        .SwellHeight = std::stod(column_values[10]),
    });
  } catch (const std::exception& ex) {
    fprintf(stderr, "Failed to parse forecast record with reason: %s\n",
            ex.what());
  }
  return 0;
}

}  // namespace

EsimoProvider::EsimoProvider(std::string resourceId,
                             std::optional<std::string> filter)
    : curl_(BuildUrl(resourceId, filter)) {}

void EsimoProvider::LoadForecasts() {
  if (!curl_.Post()) {
    fprintf(stderr, "Failed to load esimo forecast");
    throw std::runtime_error("failed to load esimo forecast");
  }
  responseBody_ = curl_.GetResponseBody();
  auto path = SaveData(responseBody_);

  std::vector<ForecastRecord> records;

  auto pFile = fopen("data", "r");
  int r = csv_parse(pFile, PushRow, (void*)(&records));
  fclose(pFile);
  if (r != 0) {
    fprintf(stderr, "Failed to parse esimo forecast");
    return;
  }
}

Forecast EsimoProvider::GetForecast() {
  return Forecast{

  };
}

std::vector<ForecastRecord> EsimoProvider::GetRecords() { return {}; }

wxString EsimoProvider::SaveData(const std::string& data) {
  wxString sep = wxFileName::GetPathSeparator();
  int64_t ts = std::chrono::system_clock::now().time_since_epoch().count();
  wxString savePath = *GetpPrivateApplicationDataLocation() + sep + "plugins" +
                      sep + "forecasts" + sep + "esimo_" + std::to_string(ts);
  std::fstream fout(savePath);
  fout << data;
  return savePath;
}

}  // namespace MarineNavi
