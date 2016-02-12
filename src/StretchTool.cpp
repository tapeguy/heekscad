// StretchTool.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "StretchTool.h"
#include "../interface/HeeksObj.h"

StretchTool::StretchTool(HeeksObj *object, const double *p, const double* shift, void* data){
	m_object = object;
	memcpy(m_pos, p, 3*sizeof(double));
	memcpy(m_shift, shift, 3*sizeof(double));
	m_undo_uses_add = false;
	m_data = data;
}

StretchTool::~StretchTool(void){
}

wxString stretch_function_string;

// Tool's virtual functions
const wxChar* StretchTool::GetTitle(){
    wxString str = m_object->GetTitle();
    if ( str.IsEmpty()) {
            str = m_object->GetTypeString();
    }
	stretch_function_string.assign(wxString::Format(_T("Stretch %s"), str));
	return stretch_function_string.c_str();
}

void StretchTool::Run(bool redo){
	m_undo_uses_add = m_object->Stretch(m_pos, m_shift, m_data);
	for(int i = 0; i<3; i++) {
	    m_new_pos[i] = m_pos[i] + m_shift[i];
	}
}

void StretchTool::RollBack(){
	if(!m_undo_uses_add){
		double unshift[3];
		for(int i = 0; i<3; i++){
			unshift[i] = 1 / m_shift[i];
		}
		m_object->Stretch(m_new_pos, unshift, m_data);
	}
}

StretchObjectsTool::StretchObjectsTool(const std::list<HeeksObj*>& list, const double* shift)
{
    m_list = list;
    memcpy(m_shift, shift, 3*sizeof(double));
}

StretchObjectsTool::~StretchObjectsTool(void)
{
}

// Tool's virtual functions
const wxChar* StretchObjectsTool::GetTitle()
{
    return _("Stretch Objects");
}

void StretchObjectsTool::Run(bool redo)
{
    double p[3];
    std::list<HeeksObj*>::iterator It;
    for(It = m_list.begin(); It != m_list.end(); It++) {
        HeeksObj* object = *It;
        for (int i = 0; i < 3; i++) {
            p[i] = 0;
        }
        object->GetStartPoint(p);
        object->Stretch(p, m_shift, NULL);
    }
    wxGetApp().WereModified(m_list);
}

void StretchObjectsTool::RollBack()
{
    double unshift[3];
    for(int i = 0; i<3; i++){
        unshift[i] = 1 / m_shift[i];
    }

    double p[3];
    std::list<HeeksObj*>::iterator It;
    for(It = m_list.begin(); It != m_list.end(); It++) {
        HeeksObj* object = *It;
        for (int i = 0; i < 3; i++) {
            p[i] = 0;
        }
        object->GetStartPoint(p);
        object->Stretch(p, unshift, NULL);
    }
    wxGetApp().WereModified(m_list);
}
