// DimensionDrawing.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "DimensionDrawing.h"
#include "Sketch.h"
#include "HLine.h"
#include "HArc.h"
#include "HeeksFrame.h"
#include "InputModeCanvas.h"

// Singleton instance
DimensionDrawing dimension_drawing;


DimensionDrawing::DimensionDrawing(void) :
 m_mode( _("mode"), _("Mode"), this)
{
    m_mode = (int)TwoPointsDimensionMode;
	m_mode.m_choices.push_back ( wxString ( _("between two points") ) );
	m_mode.m_choices.push_back ( wxString ( _("between two points, XY only") ) );
	m_mode.m_choices.push_back ( wxString ( _("between two points, X only") ) );
	m_mode.m_choices.push_back ( wxString ( _("between two points, Y only") ) );
	m_mode.m_choices.push_back ( wxString ( _("between two points, Z only") ) );
	m_mode.m_choices.push_back ( wxString ( _("orthogonal") ) );
}

DimensionDrawing::~DimensionDrawing(void)
{
}

bool DimensionDrawing::calculate_item(DigitizedPoint &end)
{
	if(end.m_type == DigitizeNoItemType)return false;

	if(TempObject() && TempObject()->GetType() != DimensionType)
	{
	    ClearObjectsMade();
	}

	gp_Trsf mat = wxGetApp().GetDrawMatrix(true);

	// make sure dimension exists
	if(TempObject()==NULL)
	{
	    int mode = m_mode;
	    AddToTempObjects(new HDimension(mat, gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0),
	                                    (DimensionMode)mode, DimensionUnitsGlobal, wxGetApp().CurrentColor()));
	}

	gp_Pnt p0, p1, p2;
	if(GetDrawStep() == 1)
	{
		p0 = GetStartPos().m_point;
		p1 = end.m_point;
		p2 = end.m_point;
	}
	else if(GetDrawStep() == 2)
	{
		p0 = GetBeforeStartPos().m_point;
		p1 = GetStartPos().m_point;
		p2 = end.m_point;
	}
	else
	{
		return false;
	}

	// double distance = p0.Distance(p1);

	((HDimension*)TempObject())->m_trsf = mat;
	((HDimension*)TempObject())->A->m_p = p0;
	((HDimension*)TempObject())->B->m_p = p1;
	((HDimension*)TempObject())->m_p2->m_p = p2;
	((HDimension*)TempObject())->m_mode = m_mode;

	return true;
}

void DimensionDrawing::GetTools(std::list<Tool*> *f_list, const wxPoint *p)
{
	Drawing::GetTools(f_list, p);
}
