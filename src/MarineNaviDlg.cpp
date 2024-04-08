#include "MarineNaviDlg.h"

#include "CheckPathCase.h"
#include "Dependencies.h"

#include <ocpn_plugin.h>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace MarineNavi {

    MarineNaviDlgBase::MarineNaviDlgBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxDialog( parent, id, title, pos, size, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE) {
        
        this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MarineNaviDlgBase::OnClose));
    }

    void MarineNaviDlgBase::Register(std::function<void(void)> func, EventType event) {
        callbacks_[event].push_back(func);
    }

    MarineNaviDlgBase::~MarineNaviDlgBase() {
        this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MarineNaviDlgBase::OnClose));
    }

    void MarineNaviDlgBase::OnClose(wxCloseEvent& event) {
        for(auto& callback : callbacks_[EventType::kClose]) {
            callback();
        }
    }

    MarineNaviMainDlg::MarineNaviMainDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const Dependencies& dependencies)
        : MarineNaviDlgBase(parent, id, title, pos, size),
            canvasWindow_(dependencies.OcpnCanvasWindow),
            checkPathCase_(dependencies.CheckPathCase) {

        auto* fgSizer11 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer11->AddGrowableCol(1);
        fgSizer11->SetFlexibleDirection(wxBOTH);
        fgSizer11->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        
        cStartLat_ = new wxTextCtrl(this, wxID_ANY);
        cStartLon_ = new wxTextCtrl(this, wxID_ANY);
        cEndLat_ = new wxTextCtrl(this, wxID_ANY);
        cEndLon_ = new wxTextCtrl(this, wxID_ANY);

        fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Start point lat")));
        fgSizer11->Add(cStartLat_, 0, wxALL|wxEXPAND, 5);
        fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("Start point lon")));
        fgSizer11->Add(cStartLon_, 0, wxALL|wxEXPAND, 5);
        fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("End point lat")));
        fgSizer11->Add(cEndLat_, 0, wxALL|wxEXPAND, 5);
        fgSizer11->Add(new wxStaticText(this, wxID_ANY, _("End point lon")));
        fgSizer11->Add(cEndLon_, 0, wxALL|wxEXPAND, 5);

        auto* fgSizer = new wxFlexGridSizer(0, 1, 0, 0);

        bCheckPath_ = new wxButton(this, wxID_ANY, _("Check path"));

        fgSizer->Add(fgSizer11);
        fgSizer->Add(bCheckPath_);
        fgSizer->AddGrowableRow(0);
        fgSizer->SetFlexibleDirection(wxBOTH);
        fgSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        SetSizerAndFit(fgSizer);
        this->Centre(wxBOTH);

        bCheckPath_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MarineNaviMainDlg::OnCheckPathClicked), NULL, this);
    }

    MarineNaviMainDlg::~MarineNaviMainDlg() {
        bCheckPath_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MarineNaviMainDlg::OnCheckPathClicked), NULL, this);
    }

    void MarineNaviMainDlg::OnCheckPathClicked(wxCommandEvent& event) {
        PathData pathData;

        auto rawLatStart = cStartLat_->GetValue();
        auto rawLonStart = cStartLon_->GetValue();
        auto rawLatEnd = cEndLat_->GetValue();
        auto rawLonEnd = cEndLon_->GetValue();

        if(!rawLatStart.ToDouble(&pathData.StartLat)
            || !rawLonStart.ToDouble(&pathData.StartLon)
            || !rawLatEnd.ToDouble(&pathData.EndLat)
            || !rawLonEnd.ToDouble(&pathData.EndLon)){

            printf("failed to parse start or end coordinates");
            return;
        }

        checkPathCase_->SetPathData(pathData);
        checkPathCase_->SetShow(true);
        RequestRefresh(canvasWindow_);
    }
}
