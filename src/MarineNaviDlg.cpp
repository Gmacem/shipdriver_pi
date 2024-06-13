#include "MarineNaviDlg.h"

#include "cases/CheckPathCase.h"
#include "cases/ForecastsLoader.h"

#include <ocpn_plugin.h>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace MarineNavi {

MarineNaviDlgBase::MarineNaviDlgBase(wxWindow* parent, wxWindowID id,
                                     const wxString& title, const wxPoint& pos,
                                     const wxSize& size)
    : wxDialog(parent, id, title, pos, size,
               wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE) {
  this->Connect(wxEVT_CLOSE_WINDOW,
                wxCloseEventHandler(MarineNaviDlgBase::OnClose));
}

void MarineNaviDlgBase::Register(std::function<void(void)> func,
                                 EventType event) {
  callbacks_[event].push_back(func);
}

MarineNaviDlgBase::~MarineNaviDlgBase() {
  this->Disconnect(wxEVT_CLOSE_WINDOW,
                   wxCloseEventHandler(MarineNaviDlgBase::OnClose));
}

void MarineNaviDlgBase::OnClose(wxCloseEvent& event) {
  for (auto& callback : callbacks_[EventType::kClose]) {
    callback();
  }
}

MarineNaviMainDlg::MarineNaviMainDlg(wxWindow* parent, wxWindowID id,
                                     const wxString& title, const wxPoint& pos,
                                     const wxSize& size,
                                     const Dependencies& dependencies)
    : MarineNaviDlgBase(parent, id, title, pos, size),
      canvasWindow_(dependencies.OcpnCanvasWindow),
      checkPathCase_(dependencies.CheckPathCase),
      forecastsLoader_(dependencies.ForecastsLoader) {
  auto* fgSizer11 = new wxFlexGridSizer(0, 2, 0, 0);
  fgSizer11->AddGrowableCol(1);
  fgSizer11->SetFlexibleDirection(wxBOTH);
  fgSizer11->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  cStartLat_ = new wxTextCtrl(this, wxID_ANY);
  cStartLon_ = new wxTextCtrl(this, wxID_ANY);
  cEndLat_ = new wxTextCtrl(this, wxID_ANY);
  cEndLon_ = new wxTextCtrl(this, wxID_ANY);
  cShipDraft_ = new wxTextCtrl(this, wxID_ANY);
  cMaxWave_ = new wxTextCtrl(this, wxID_ANY);
  cPathToFile_ = new wxTextCtrl(this, wxID_ANY);

  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Start point lat")));
  fgSizer11->Add(cStartLat_, 0, wxALL | wxEXPAND, 5);
  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Start point lon")));
  fgSizer11->Add(cStartLon_, 0, wxALL | wxEXPAND, 5);
  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("End point lat")));
  fgSizer11->Add(cEndLat_, 0, wxALL | wxEXPAND, 5);
  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("End point lon")));
  fgSizer11->Add(cEndLon_, 0, wxALL | wxEXPAND, 5);

  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Ship draft")));
  fgSizer11->Add(cShipDraft_, 0, wxALL | wxEXPAND, 5);
  fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Max wave")));
  fgSizer11->Add(cMaxWave_, 0, wxALL | wxEXPAND, 5);
  fgSizer11->Add(
      new wxStaticText(this, wxID_ANY, _("Path to depth data file")));
  fgSizer11->Add(cPathToFile_, 0, wxALL | wxEXPAND, 5);

  auto* fgSizer = new wxFlexGridSizer(0, 1, 0, 0);

  bCheckPath_ = new wxButton(this, wxID_ANY, _("Check path"));
  bLoadForecasts_ = new wxButton(this, wxID_ANY, _("Download forecasts"));

  fgSizer->Add(fgSizer11);
  fgSizer->Add(bCheckPath_);
  fgSizer->Add(bLoadForecasts_);
  fgSizer->AddGrowableRow(0);
  fgSizer->SetFlexibleDirection(wxBOTH);
  fgSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  SetSizerAndFit(fgSizer);
  this->Centre(wxBOTH);

  bCheckPath_->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(MarineNaviMainDlg::OnCheckPathClicked), NULL, this);
  bLoadForecasts_->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(MarineNaviMainDlg::OnLoadForecastsClicked), NULL,
      this);
}

MarineNaviMainDlg::~MarineNaviMainDlg() {
  bCheckPath_->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(MarineNaviMainDlg::OnCheckPathClicked), NULL, this);
  bLoadForecasts_->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(MarineNaviMainDlg::OnLoadForecastsClicked), NULL,
      this);
}

void MarineNaviMainDlg::OnCheckPathClicked(wxCommandEvent& event) {
  PathData pathData;

  auto rawLatStart = cStartLat_->GetValue();
  auto rawLonStart = cStartLon_->GetValue();
  auto rawLatEnd = cEndLat_->GetValue();
  auto rawLonEnd = cEndLon_->GetValue();
  auto rawShipDraft = cShipDraft_->GetValue();
  auto rawMaxWave = cMaxWave_->GetValue();
  pathData.PathToDepthFile = cPathToFile_->GetValue();

  if (!rawLatStart.ToDouble(&pathData.Start.Lat) ||
      !rawLonStart.ToDouble(&pathData.Start.Lon) ||
      !rawLatEnd.ToDouble(&pathData.End.Lat) ||
      !rawLonEnd.ToDouble(&pathData.End.Lon)) {
    printf("failed to parse start or end coordinates");  // TODO notify somehow
    return;
  }

  if (!rawShipDraft.empty()) {
    double shipDraft;
    if (!rawShipDraft.ToDouble(&shipDraft)) {
      printf("failed to parse ship draft");  // TODO notify somehow
      return;
    }
    pathData.ShipDraft = shipDraft;
  }
  if (!rawMaxWave.empty()) {  // TODO unite with rawShip
    double maxWave;
    if (!rawMaxWave.ToDouble(&maxWave)) {
      printf("failed to parse max wave");  // TODO notify somehow
      return;
    }
    pathData.MaxWaveHeight = maxWave;
  }

  checkPathCase_->SetPathData(pathData);
  checkPathCase_->SetShow(true);
  checkPathCase_->CrossDetect();
  RequestRefresh(canvasWindow_);
}

void MarineNaviMainDlg::OnLoadForecastsClicked(wxCommandEvent& event) {
  try {
    forecastsLoader_->Load();
  } catch (const std::exception& ex) {
    fprintf(stderr, "Failed esimo forecasts loading with reason %s\n",
            ex.what());
  }
}

}  // namespace MarineNavi
