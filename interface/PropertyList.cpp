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

void PropertyList::Clear()
{
    m_value.clear();
    DomainObject::RemoveAllProperties();
}

bool PropertyList::AddProperty(Property *prop)
{
    if ( ! DomainObject::AddProperty(prop) ) {
        return false;
    }
	m_value.push_back(prop);
	return true;

}

void PropertyList::RemoveProperty(Property *prop)
{
    m_value.remove(prop);
    DomainObject::RemoveProperty(prop);
}

bool PropertyList::OnPrePropertySet(Property& prop)
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPrePropertySet(prop);
    return TRUE;
}

void PropertyList::OnPropertySet(Property& prop)
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        owner->OnPropertySet(prop);
}

void PropertyList::OnPropertyEdit(Property& prop)
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPropertyEdit(prop);
}


void PropertyList::operator = ( const Property& prop )
{
    const PropertyList * value = &(const PropertyList&)prop;
    SetValue(value->m_value);
}

Property * PropertyList::Clone ( ) const
{
    PropertyList * prop = new PropertyList();
    prop->operator =(*this);
    return (Property *)prop;
}

void PropertyList::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    for ( TiXmlElement* c = element->FirstChildElement ( ); c; c = c->NextSiblingElement ( ) )
    {
        wxString name = c->Value ( );
        std::list<Property *>::const_iterator It;
        for ( It = m_value.begin(); It != m_value.end(); It++ )
        {
            Property * prop = *It;
            if ( prop->GetXmlName() == name )
            {
                prop->ReadFromXmlElement ( c );
                break;
            }
        }
    }

}

void PropertyList::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( Property::GetName() );
    element->LinkEndChild ( prop_element );
    std::list<Property*>::const_iterator It;
    for(It = m_value.begin(); It != m_value.end(); It++)
    {
        Property* property = *It;
        property->DefaultWriteToXmlElement ( prop_element );
    }
}
