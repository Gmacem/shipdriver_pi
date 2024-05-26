#pragma once

#include <ctime>
#include <optional>

namespace MarineNavi {

enum class ForecastsSource {
  kEsimo = 0,
};

struct Forecast {
  time_t StartedAt;
  time_t FinishedAt;
  ForecastsSource Source;
};

struct ForecastRecord {
  time_t StartedAt;
  time_t Date;
  double Lat;
  double Lon;
  std::optional<double> WaveHeight;
  std::optional<double> SwellHeight;
};

}  // namespace MarineNavi
