// PropertyList.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "PropertyList.h"


PropertyList::PropertyList()
:PropertyTmpl<std::list< Property* > >()
{
}

PropertyList::PropertyList(const wxChar* t, MutableObject* object)
:PropertyTmpl<std::list< Property* > >(t, std::list<Property*>(), object)
{
}

void PropertyList::SetHighlighted(bool value)
{
	m_highlighted = value;
	std::list< Property* >::iterator It;
	for(It = m_value.begin(); It != m_value.end(); It++)
	{
		Property* property = *It;
		property->SetHighlighted(value);
	}
}

void PropertyList::SetReadOnly(bool value)
{
	m_readonly = value;
	std::list< Property* >::iterator It;
	for(It = m_value.begin(); It != m_value.end(); It++)
	{
		Property* property = *It;
		property->SetReadOnly(value);
	}
}

void PropertyList::AddProperty(Property *prop)
{
	m_value.push_back(prop);
	MutableObject::AddProperty(prop);
}

void PropertyList::OnPropertySet(Property *prop)
{
	if(m_object) m_object->OnPropertySet(prop);
}

void PropertyList::OnPropertyEdit(Property *prop)
{
	if(m_object) m_object->OnPropertyEdit(prop);
}

void PropertyList::OnPropertySelect(Property *prop)
{
	if(m_object) m_object->OnPropertySelect(prop);
}

void PropertyList::OnPropertiesApply()
{
	if(m_object) m_object->OnPropertiesApply();
}
