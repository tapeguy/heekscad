// IdNamedObj.cpp

#include <stdafx.h>

#include "IdNamedObj.h"

static wxString temp_pattern_string;


void IdNamedObj::OnSetID(int id)
{
    if (m_title_made_from_id) {
        temp_pattern_string = wxString::Format(_T("%s %d"), GetTypeString(), id);
        HeeksObj::SetTitle(temp_pattern_string);
    }
    HeeksObj::OnSetID(id);
}

void IdNamedObj::OnSetTitle(const wxString& title)
{
    temp_pattern_string = wxString::Format(_T("%s %d"), GetTypeString(), GetID());
    m_title_made_from_id = (title == temp_pattern_string);
    if ( ! m_title_made_from_id ) {
        OnEditString(title);
    }
    HeeksObj::OnSetTitle(title);
}


void IdNamedObj::OnEditString(const wxChar* str)
{
}
