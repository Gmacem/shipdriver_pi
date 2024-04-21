#pragma once

#include <pidc.h>

#include "Dependencies.h"
#include "CheckPathCase.h"

namespace MarineNavi {

class RenderOverlay {
public:
    RenderOverlay(Dependencies& deps): checkPathCase_(deps.CheckPathCase) {
    }

    bool Render(piDC& dc, PlugIn_ViewPort *vp) { 
        if (checkPathCase_->IsShow()) {
            auto pathData = checkPathCase_->GetPathData();
            RenderCheckPath(dc, vp, pathData);
            return true;
        }
        return false;
    }

private:
    void RenderCheckPath(piDC& dc, PlugIn_ViewPort *vp, const PathData& pathData) {
        dc.SetPen(*wxBLACK); // reset pen
        dc.SetBrush( *wxTRANSPARENT_BRUSH); // reset brush
        dc.SetPen(wxPen(wxColor(0, 0, 0)));
        wxPen pen = dc.GetPen();
        pen.SetWidth(5);
        dc.SetPen(pen);

        wxPoint2DDouble ptD;
        GetDoubleCanvasPixLL(vp, &ptD, pathData.Start.Lat, pathData.Start.Lon);
        wxPoint pt1, pt2;
        pt1.x = round(wxRound(ptD.m_x));
        pt1.y = round(wxRound(ptD.m_y));
        GetDoubleCanvasPixLL(vp, &ptD, pathData.End.Lat, pathData.End.Lon);
        pt2.x = round(wxRound(ptD.m_x));
        pt2.y = round(wxRound(ptD.m_y));
        dc.DrawLine(pt1.x, pt1.y, pt2.x, pt2.y);

        auto cross = checkPathCase_->CrossDetect(pathData);
        if (cross.has_value()) {
            fprintf(stderr, "Draw circle %f %f\n", cross->m_x, cross->m_y);
            wxPoint2DDouble crossCenter;
            GetDoubleCanvasPixLL(vp, &crossCenter, cross->m_x, cross->m_y);
            fprintf(stderr, "Draw circle2 %f %f\n", crossCenter.m_x, crossCenter.m_y);
            dc.DrawCircle(round(crossCenter.m_x), round(crossCenter.m_y), 10);
        }
    }

private:
std::shared_ptr<CheckPathCase> checkPathCase_;

};

} // namespace MarineNavi