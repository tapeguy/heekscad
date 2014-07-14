// PropertyList.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "PropertyList.h"


PropertyList::PropertyList()
:PropertyTmpl<std::list< Property* > >()
{
}

PropertyList::PropertyList(const wxChar* name, const wxChar* title, DomainObject* owner)
:PropertyTmpl<std::list< Property* > >(name, title, owner)
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
	m_value.push_back((Property *)prop);
	DomainObject::AddProperty(prop);
}

bool PropertyList::OnPropertySet(Property& prop)
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPropertySet(*this);
    return TRUE;
}

void PropertyList::OnPropertyEdit(Property& prop)
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPropertyEdit(*this);
}


void PropertyList::operator = ( const Property& prop )
{
    const PropertyList * value = &(const PropertyList&)prop;
    SetValue(value->m_value);
}

Property * PropertyList::Clone ( ) const
{
    PropertyList * prop = new PropertyList();
    *prop = *this;
    return (Property *)prop;
}
