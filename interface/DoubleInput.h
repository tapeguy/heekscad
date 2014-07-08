// DoubleInput.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/InputMode.h"
#include "../interface/LeftAndRight.h"

class PropertyDouble;

class CDoubleInput: public CInputMode, public CLeftAndRight
{
protected:
	PropertyDouble * m_value;

public:
	wxString m_title;
	static bool m_success;

	CDoubleInput(const wxChar* prompt, const wxChar* value_name, double initial_value);
	~CDoubleInput();

	double GetValue();

	// virtual functions for InputMode
	const wxChar* GetTitle();
	void OnMouse( wxMouseEvent& event );
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

protected:
	CDoubleInput();
};

class CLengthInput: public CDoubleInput
{
public:
	CLengthInput(const wxChar* prompt, const wxChar* value_name, double initial_value);
};

