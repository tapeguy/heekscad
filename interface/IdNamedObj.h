// IdNamedObj.h
/*
 * Copyright (c) 2013, Dan Heeks
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

#pragma once

#include "HeeksObj.h"

class IdNamedObj: public HeeksObj
{
public:
	bool m_title_made_from_id;

	IdNamedObj(int obj_type, const wxChar* title = NULL)
	: HeeksObj(obj_type)
	{
        if(title) {
            m_title_made_from_id = false;
            SetTitle(title);
        }
        else {
            m_title_made_from_id = true;
        }
	}

	// HeeksObj's virtual functions
	void OnSetID(int id);
    void OnSetTitle(const wxString& title);
    bool CanEditString(void)const{return true;}
    void OnEditString(const wxChar* str);
};
