#include "Dependencies.h"

#include "cases/CheckPathCase.h"
#include "cases/ForecastsLoader.h"
#include "clients/DbClient.h"
#include "clients/Esimo.h"
#include "RenderOverlay.h"

namespace marine_navi {

Dependencies CreateDependencies(wxWindow* ocpnCanvasWindow) {
  Dependencies deps;
  deps.OcpnCanvasWindow = ocpnCanvasWindow;
  deps.Db = CreateDatabase("marinenavi.db");
  deps.DbClient = std::make_shared<DbClient>(deps.Db);
  deps.ForecastsLoader =
      std::make_shared<marine_navi::ForecastsLoader>(deps.DbClient);
  deps.CheckPathCase =
      std::make_shared<marine_navi::CheckPathCase>(deps.DbClient);
  deps.RenderOverlay = std::make_shared<marine_navi::RenderOverlay>(deps);
  return deps;
}

}  // namespace marine_navi
