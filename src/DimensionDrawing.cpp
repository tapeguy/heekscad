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
	temp_object = NULL;
}

DimensionDrawing::~DimensionDrawing(void)
{
}

bool DimensionDrawing::calculate_item(DigitizedPoint &end)
{
	if(end.m_type == DigitizeNoItemType)return false;

	if(temp_object && temp_object->GetType() != DimensionType){
		delete temp_object;
		temp_object = NULL;
		temp_object_in_list.clear();
	}

	gp_Trsf mat = wxGetApp().GetDrawMatrix(true);

	// make sure dimension exists
	if(!temp_object){
		int mode = m_mode;
		temp_object = new HDimension(mat, gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0),
		                             (DimensionMode)mode, DimensionUnitsGlobal, wxGetApp().CurrentColor());
		if(temp_object)temp_object_in_list.push_back(temp_object);
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

	((HDimension*)temp_object)->m_trsf = mat;
	((HDimension*)temp_object)->A->m_p = p0;
	((HDimension*)temp_object)->B->m_p = p1;
	((HDimension*)temp_object)->m_p2->m_p = p2;
	((HDimension*)temp_object)->m_mode = m_mode;

	return true;
}

void DimensionDrawing::clear_drawing_objects(int mode)
{
	if(temp_object && mode == 2)delete temp_object;
	temp_object = NULL;
	temp_object_in_list.clear();
}

void DimensionDrawing::StartOnStep3(HDimension* object)
{
	wxGetApp().SetInputMode(this);
	temp_object = object;
	temp_object_in_list.push_back(object);
	set_draw_step_not_undoable(2);
	current_view_stuff->before_start_pos.m_point = object->A->m_p;
	current_view_stuff->start_pos.m_point = object->B->m_p;

	m_mode = object->m_mode;
}

void DimensionDrawing::GetTools(std::list<Tool*> *f_list, const wxPoint *p){
	Drawing::GetTools(f_list, p);
}
