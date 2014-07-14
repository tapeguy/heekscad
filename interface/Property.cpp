// Property.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "Property.h"

#ifdef HEEKSCAD
	#define VIEW_UNITS (wxGetApp().m_view_units)
#else
	#define VIEW_UNITS (heeksCAD->GetViewUnits())
#endif



void PropertyCheck::operator = ( const Property& prop )
{
    const PropertyCheck * value = &(const PropertyCheck&)prop;
    SetValue(value->m_value);
}

Property * PropertyCheck::Clone ( ) const
{
    PropertyCheck * rtn = new PropertyCheck(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyChoice::operator = ( const Property& prop )
{
    const PropertyChoice * value = &(const PropertyChoice&)prop;
    SetValue(value->m_value);
}

Property * PropertyChoice::Clone ( ) const
{
    PropertyChoice * rtn = new PropertyChoice(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyColor::operator = ( const Property& prop )
{
    const PropertyColor * value = &(const PropertyColor&)prop;
    SetValue(value->m_value);
}

Property * PropertyColor:: Clone ( ) const
{
    PropertyColor * rtn = new PropertyColor(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyDouble::operator = ( const Property& prop )
{
    const PropertyDouble * value = &(const PropertyDouble&)prop;
    SetValue(value->m_value);
}

Property * PropertyDouble::Clone ( ) const
{
    PropertyDouble * rtn = new PropertyDouble(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

PropertyLength::PropertyLength()
:PropertyDouble()
{
}

PropertyLength::PropertyLength(double initial_value)
:PropertyDouble()
{
	m_value = initial_value / VIEW_UNITS;
}

PropertyLength::PropertyLength(const wxChar* name, const wxChar* title, DomainObject* owner)
:PropertyDouble(name, title, owner)
{
}

void PropertyLength::SetValue(const double& value)
{
	m_value = value / VIEW_UNITS;
	CallSetFunction();
}

double PropertyLength::operator=(double value)
{
	SetValue(value);
	adjusted_length = m_value * VIEW_UNITS;
	return adjusted_length;
}

PropertyLength::operator const double&() const
        {
	adjusted_length = m_value * VIEW_UNITS;
	return adjusted_length;
}

void PropertyLength::operator = ( const Property& prop )
{
    const PropertyLength * value = &(const PropertyLength&)prop;
    SetValue(value->m_value);
}

Property * PropertyLength::Clone ( ) const
{
    PropertyLength * rtn = new PropertyLength(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    rtn->adjusted_length = adjusted_length;
    return rtn;
}

void PropertyInt::operator = ( const Property& prop )
{
    const PropertyInt * value = &(const PropertyInt&)prop;
    SetValue(value->m_value);
}

Property * PropertyInt::Clone ( ) const
{
    PropertyInt * rtn = new PropertyInt(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyString::operator = ( const Property& prop )
{
    const PropertyString * value = &(const PropertyString&)prop;
    SetValue(value->m_value);
}

Property * PropertyString::Clone ( ) const
{
    PropertyString * rtn = new PropertyString(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyFile::operator = ( const Property& prop )
{
    const PropertyFile * value = &(const PropertyFile&)prop;
    SetValue(value->m_value);
}

Property * PropertyFile::Clone ( ) const
{
    PropertyFile * rtn = new PropertyFile(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

double PropertyVector::X(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.X() / VIEW_UNITS: m_value.X();
}

double PropertyVector::Y(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Y() / VIEW_UNITS: m_value.Y();
}

double PropertyVector::Z(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Z() / VIEW_UNITS: m_value.Z();
}

void PropertyVector::operator = ( const Property& prop )
{
    const PropertyVector * value = &(const PropertyVector&)prop;
    SetValue(value->m_value);
}

Property * PropertyVector::Clone ( ) const
{
    PropertyVector * rtn = new PropertyVector(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

double PropertyVertex::X(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.X() / VIEW_UNITS: m_value.X();
}

double PropertyVertex::Y(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Y() / VIEW_UNITS: m_value.Y();
}

double PropertyVertex::Z(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Z() / VIEW_UNITS: m_value.Z();
}

void PropertyVertex::operator = ( const Property& prop )
{
    const PropertyVertex * value = &(const PropertyVertex&)prop;
    SetValue(value->m_value);
}

Property * PropertyVertex::Clone ( ) const
{
    PropertyVertex * rtn = new PropertyVertex(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyVertex2d::operator = ( const Property& prop )
{
    const PropertyVertex2d * value = &(const PropertyVertex2d&)prop;
    SetValue(value->m_value);
}

Property * PropertyVertex2d::Clone ( ) const
{
    PropertyVertex2d * rtn = new PropertyVertex2d(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyTrsf::operator = ( const Property& prop )
{
    const PropertyTrsf * value = &(const PropertyTrsf&)prop;
    SetValue(value->m_value);
}

Property * PropertyTrsf::Clone ( ) const
{
    PropertyTrsf * rtn = new PropertyTrsf(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}

void PropertyCoord::operator = ( const Property& prop )
{
    const PropertyCoord * value = &(const PropertyCoord&)prop;
    SetValue(value->m_value);
}

Property * PropertyCoord::Clone ( ) const
{
    PropertyCoord * rtn = new PropertyCoord(this->GetName(), m_title, this->GetOwner());
    rtn->m_value = m_value;
    return rtn;
}
