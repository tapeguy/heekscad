// IdNamedObjList.cpp

#include <stdafx.h>

#include "IdNamedObjList.h"

static wxString temp_pattern_string;

void IdNamedObjList::OnSetID(ObjectId_t id)
{
    if (m_title_made_from_id) {
        temp_pattern_string = wxString::Format(_T("%s %d"), GetTypeString(), (int)id);
        HeeksObj::SetTitle(temp_pattern_string);
    }
    HeeksObj::OnSetID(id);
}

void IdNamedObjList::OnSetTitle(const wxString& title)
{
    if (!title.IsEmpty()) {
        temp_pattern_string = wxString::Format(_T("%s %d"), GetTypeString(), GetID());
        m_title_made_from_id = (title == temp_pattern_string);
        if ( ! m_title_made_from_id ) {
            OnEditString(title);
        }
    }
    HeeksObj::OnSetTitle(title);
}

void IdNamedObjList::OnEditString(const wxChar* str)
{
}
