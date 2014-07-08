// DoubleInput.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "DoubleInput.h"
#include "Property.h"

class CInputApply:public Tool{
public:
	void Run(){
		CDoubleInput::m_success = true;
		wxGetApp().ExitMainLoop();
	}
	const wxChar* GetTitle(){return _("Apply");}
	wxString BitmapPath(){return _T("apply");}
	const wxChar* GetToolTip(){return _("Accept value and continue");}
};

CInputApply input_apply;

class CInputCancel:public Tool{
public:
	void Run(){wxGetApp().ExitMainLoop();}
	const wxChar* GetTitle(){return _("Cancel");}
	wxString BitmapPath(){return _T("cancel");}
	const wxChar* GetToolTip(){return _("Cancel operation");}
};

CInputCancel input_cancel;

// static
bool CDoubleInput::m_success = false;


CDoubleInput::CDoubleInput()
{
}

CDoubleInput::CDoubleInput(const wxChar* prompt, const wxChar* value_name, double initial_value)
{
        m_value = new PropertyDouble(value_name, initial_value, this);
	m_title.assign(prompt);
	m_success = false;
}

CDoubleInput::~CDoubleInput()
{
	delete m_value;
}

const wxChar* CDoubleInput::GetTitle()
{
	return m_title.c_str();
}

double CDoubleInput::GetValue()
{
	double rtn = *m_value;
	return rtn;
}

void CDoubleInput::OnMouse( wxMouseEvent& event )
{
	bool event_used = false;
	if(LeftAndRightPressed(event, event_used))
		wxGetApp().ExitMainLoop();
}

void CDoubleInput::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	// add a do it now button
	t_list->push_back(&input_apply);
	t_list->push_back(&input_cancel);
}


CLengthInput::CLengthInput(const wxChar* prompt, const wxChar* value_name, double initial_value)
{
	m_title.assign(prompt);
	m_value = new PropertyLength(value_name, initial_value, this);
	m_success = false;
}
