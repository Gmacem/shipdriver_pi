#pragma once

#ifndef _SDR_PI_H_
#define _SDR_PI_H_

#include "CheckPathCase.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/glcanvas.h>
#endif // precompiled headers

#include <wx/fileconf.h>
#include <wx/datetime.h>
#include <wx/tokenzr.h>

#include "config.h"

#include "json/reader.h"
#include "json/writer.h"

#include "ocpn_plugin.h" //Required for OCPN plugin functions

#include "MarineNaviDlg.h"
#include "RenderOverlay.h"

#include "pidc.h"

// Define minimum and maximum versions of the grib plugin supported
#define GRIB_MAX_MAJOR 4
#define GRIB_MAX_MINOR 1
#define GRIB_MIN_MAJOR 4
#define GRIB_MIN_MINOR 1

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define ShipDriver_TOOL_POSITION                                               \
    -1 // Request default positioning of toolbar tool

class MarineNaviPi : public opencpn_plugin_118 {
public:
    MarineNaviPi(void* ppimgr);
    ~MarineNaviPi(void);

    //    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap* GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();


    bool RenderOverlay(wxDC &wxdc, PlugIn_ViewPort *vp) override {
        piDC dc(wxdc);

        return renderOverlay_->Render(dc, vp);
    }

    bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp) override {
        piDC dc;
        dc.SetVP(vp);

        return renderOverlay_->Render(dc, vp);
    }

    int GetToolbarToolCount(void) { return 1; }
    void OnToolbarToolCallback(int id);

    void OnMainDlgClose();

    static wxString StandardPath();

    wxBitmap panelBitmap_;

private:
    wxWindow* parentWindow_;
    std::shared_ptr<MarineNavi::MarineNaviMainDlg> dlg_;
    std::shared_ptr<MarineNavi::RenderOverlay> renderOverlay_;

    int toolId_;
    bool showDlg_;
};

#endif
