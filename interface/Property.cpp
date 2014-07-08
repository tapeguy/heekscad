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

PropertyLength::PropertyLength()
:PropertyDouble()
{
}

PropertyLength::PropertyLength(double initial_value)
:PropertyDouble()
{
	m_value = initial_value / VIEW_UNITS;
}

PropertyLength::PropertyLength(const wxChar* t, double initial_value, MutableObject* object)
:PropertyDouble(t, initial_value / VIEW_UNITS, object)
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
