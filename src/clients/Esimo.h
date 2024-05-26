#pragma once

#include "common/csv_parser.h"

#include "ForecastsProvider.h"

#include "ocpn_plugin.h"

#include <wx/curl/http.h>

#include <optional>
#include <string>
#include <vector>

namespace MarineNavi {

class EsimoProvider : public IForecastsProvider {
public:
  EsimoProvider(std::string resourceId, std::optional<std::string> filter);

  void LoadForecasts() override;
  Forecast GetForecast() override;
  std::vector<ForecastRecord> GetRecords() override;
  ~EsimoProvider() override = default;

private:
  wxString SaveData(const std::string& data);

private:
  wxCurlHTTP curl_;
  std::string url;
  std::string responseBody_;
};

}  // namespace MarineNavi
