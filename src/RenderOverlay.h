#pragma once

#include <pidc.h>

#include "cases/CheckPathCase.h"
#include "Dependencies.h"

#include <wx/wx.h>

namespace marine_navi {

class RenderOverlay {
public:
  RenderOverlay(Dependencies& deps);

  bool Render(piDC& dc, PlugIn_ViewPort* vp);

private:
  void RenderCheckPath(piDC& dc, PlugIn_ViewPort* vp, const PathData& pathData);

private:
  std::shared_ptr<CheckPathCase> checkPathCase_;

  bool checkPathCalculated_;
  std::optional<wxPoint2DDouble> checkPathResult_;
};

}  // namespace marine_navi
