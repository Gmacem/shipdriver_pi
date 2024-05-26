#pragma once

#include <pidc.h>

#include "cases/CheckPathCase.h"
#include "Dependencies.h"

namespace MarineNavi {

class RenderOverlay {
public:
  RenderOverlay(Dependencies& deps);

  bool Render(piDC& dc, PlugIn_ViewPort* vp);

private:
  void RenderCheckPath(piDC& dc, PlugIn_ViewPort* vp, const PathData& pathData);

private:
  std::shared_ptr<CheckPathCase> checkPathCase_;
};

}  // namespace MarineNavi
