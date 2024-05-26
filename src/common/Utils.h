#pragma once

#include <ctime>
#include <string>
#include <time.h>

namespace MarineNavi {
namespace Utils {

struct Point {
  double& X();
  double& Y();

  double Lat;
  double Lon;
};

Point operator+(const Point& lhs, const Point& rhs);
Point operator-(const Point& lhs, const Point& rhs);
Point operator*(const Point& p, double x);
Point operator*(double x, const Point& p);

time_t ParseDate(const std::string& repr,
                 const std::string& format = "%Y-%m-%d %H:%M:%S");

}  // namespace Utils
}  // namespace MarineNavi