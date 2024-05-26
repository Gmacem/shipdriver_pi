#include "Utils.h"

namespace MarineNavi {
namespace Utils {

double& Point::X() { return Lon; }

double& Point::Y() { return Lat; }

Point operator+(const Point& lhs, const Point& rhs) {
  return Point{lhs.Lat + rhs.Lat, lhs.Lon + rhs.Lon};
}

Point operator-(const Point& lhs, const Point& rhs) {
  return Point{lhs.Lat - rhs.Lat, lhs.Lon - rhs.Lon};
}

Point operator*(const Point& p, double x) {
  return Point{p.Lat * x, p.Lon * x};
}

Point operator*(double x, const Point& p) {
  return Point{p.Lat * x, p.Lon * x};
}

time_t ParseDate(const std::string& repr, const std::string& format) {
  struct tm tm;
  strptime(repr.data(), format.data(), &tm);
  return mktime(&tm);
}

}  // namespace Utils
}  // namespace MarineNavi
