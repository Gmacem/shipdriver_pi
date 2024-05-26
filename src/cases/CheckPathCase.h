#pragma once

#include "entities/DepthGrid.h"
#include "common/Utils.h"

#include <ocpn_plugin.h>

#include <wx/wx.h>

#include <mutex>
#include <optional>

namespace MarineNavi {

struct PathData {
  Utils::Point Start;
  Utils::Point End;

  std::optional<double> ShipDraft;
  std::optional<std::string> PathToDepthFile;
};

class CheckPathCase {
  using Point = Utils::Point;

public:
  CheckPathCase();
  void SetPathData(const PathData& pathData);
  const PathData& GetPathData();
  void SetShow(bool show);

  bool IsShow();

  bool CheckLandIntersection(const Point& p1, const Point& p2) const;
  bool CheckDepth(const DepthGrid& grid, const Point& p, double draft) const;
  std::optional<wxPoint2DDouble> CrossDetect(const PathData& pathData_) const;

private:
  std::mutex mutex_;
  PathData pathData_;
  bool show_;
};

}  // namespace MarineNavi
