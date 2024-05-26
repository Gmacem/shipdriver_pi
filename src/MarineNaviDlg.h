#pragma once

#include "Dependencies.h"

#include <wx/dialog.h>
#include <wx/textctrl.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace MarineNavi {

class MarineNaviDlgBase : public wxDialog {
public:
  enum class EventType {
    kClose,
  };

public:
  MarineNaviDlgBase(wxWindow* parent, wxWindowID id, const wxString& title,
                    const wxPoint& pos, const wxSize& size);
  void Register(std::function<void(void)> func, EventType event);
  virtual ~MarineNaviDlgBase();

protected:
  virtual void OnClose(wxCloseEvent& event);

protected:
  std::unordered_map<EventType, std::vector<std::function<void(void)>>>
      callbacks_;
};

class MarineNaviMainDlg : public MarineNaviDlgBase {
public:
  MarineNaviMainDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                    const wxPoint& pos, const wxSize& size,
                    const Dependencies& dependencies);
  ~MarineNaviMainDlg() override;

private:
  void OnCheckPathClicked(wxCommandEvent& event);

private:
  wxWindow* canvasWindow_;
  std::shared_ptr<CheckPathCase> checkPathCase_;

  wxTextCtrl* cStartLat_;
  wxTextCtrl* cStartLon_;
  wxTextCtrl* cEndLat_;
  wxTextCtrl* cEndLon_;
  wxTextCtrl* cShipDraft_;
  wxTextCtrl* cPathToFile_;  // TODO use wxFileDialog or something else

  wxButton* bCheckPath_;
};

};  // namespace MarineNavi
