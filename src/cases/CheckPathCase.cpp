#include "CheckPathCase.h"

namespace MarineNavi {

CheckPathCase::CheckPathCase() : mutex_(), pathData_(), show_(false) {}

void CheckPathCase::SetPathData(const PathData& pathData) {
  std::lock_guard lock(mutex_);
  pathData_ = pathData;
}

const PathData& CheckPathCase::GetPathData() {
  std::lock_guard lock(mutex_);
  return pathData_;
}

void CheckPathCase::SetShow(bool show) {
  std::lock_guard lock(mutex_);
  fprintf(stderr, "Trying set show\n");
  show_ = show;
}

bool CheckPathCase::IsShow() {
  std::lock_guard lock(mutex_);
  return show_;
}

bool CheckPathCase::CheckLandIntersection(const Point& p1,
                                          const Point& p2) const {
  static constexpr double EPS = 1e-5;

  Point minCorner{std::min(p1.Lat, p2.Lat), std::min(p1.Lon, p2.Lon)};
  Point maxCorner{std::max(p1.Lat, p2.Lat), std::max(p1.Lon, p2.Lon)};
  minCorner.Lat -= EPS;
  minCorner.Lon -= EPS;
  maxCorner.Lat += EPS;
  maxCorner.Lon += EPS;

  return PlugIn_GSHHS_CrossesLand(minCorner.Lat, minCorner.Lon, maxCorner.Lat,
                                  maxCorner.Lon);
}

bool CheckPathCase::CheckDepth(const DepthGrid& grid, const Point& p,
                               double draft) const {
  auto depth = grid.GetDepth(p.Lat, p.Lon);

  if (depth.has_value()) {
    return -depth.value() > draft;  // TODO perhaps it needs to be taken into
                                    // account with some margin
  }

  return true;
}

std::optional<wxPoint2DDouble> CheckPathCase::CrossDetect(
    const PathData& pathData_) const {
  static constexpr int ITER_NUM = 50;
  const Point start = pathData_.Start;
  const Point end = pathData_.End;
  std::optional<DepthGrid> grid;
  if (pathData_.PathToDepthFile.has_value() &&
      pathData_.ShipDraft.has_value()) {
    grid = DepthGrid(pathData_.PathToDepthFile.value());
  }

  if (!CheckLandIntersection(start, end)) {
    return std::nullopt;
  }

  Point vec = end - start;

  for (int i = 0; i <= ITER_NUM; ++i) {
    double k = static_cast<double>(i) / ITER_NUM;
    Point vv = vec * k;
    Point p = start + vv;

    if (grid.has_value() &&
        !CheckDepth(grid.value(), p, pathData_.ShipDraft.value())) {
      return wxPoint2DDouble(p.Lat, p.Lon);
    }

    if (CheckLandIntersection(pathData_.Start, p)) {
      return wxPoint2DDouble(p.Lat, p.Lon);
    }
  }
  return std::nullopt;
}

}  // namespace MarineNavi
