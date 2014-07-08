// DigitizeMode.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/InputMode.h"
#include "DigitizedPoint.h"

class CViewPoint;
class PointOrWindow;

class DigitizeMode:public CInputMode{
private:
	PointOrWindow *point_or_window;
	DigitizedPoint lbutton_point;
	std::set<HeeksObj*> m_only_coords_set;

	PropertyLength m_x;
	PropertyLength m_y;
	PropertyLength m_z;
	PropertyString m_offset;

	DigitizedPoint digitize1(const wxPoint &input_point);

public:
	DigitizedPoint digitized_point;
	DigitizedPoint reference_point;	// the last point the operator explicitly defined (as opposed to mouse movements over the graphics canvas)
	bool m_doing_a_main_loop;
	wxString m_prompt_when_doing_a_main_loop;
	void(*m_callback)(const double*);

	DigitizeMode();
	virtual ~DigitizeMode(void);

	// InputMode's virtual functions
	void InitializeProperties();
	void OnPropertyEdit(Property * prop);
	void GetProperties(std::list<Property *> *list);
	const wxChar* GetTitle();
	const wxChar* GetHelpText();
	void OnMouse( wxMouseEvent& event );
	void OnKeyDown(wxKeyEvent& event);
	bool OnModeChange(void);
	void OnFrontRender();
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	DigitizedPoint DigitizePoint(const wxPoint &point);
	DigitizedPoint DigitizeRay(const gp_Lin &ray, gp_Pln plane = gp_Pln(gp_Pnt(0, 0, 0), gp_Vec(0, 0, 1)));
	void SetOnlyCoords(HeeksObj* object, bool onoff);
	bool OnlyCoords(HeeksObj* object);
};
