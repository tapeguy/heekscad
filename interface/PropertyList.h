// PropertyList.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined PropertyList_HEADER
#define PropertyList_HEADER

#include "Property.h"

class PropertyList : public PropertyTmpl<std::list< Property* > >, public MutableObject
{
public:
	PropertyList();
	PropertyList(const wxChar* t, MutableObject* object);

        const PropertyList& operator=(const PropertyList& value) { SetValue(value.m_value); return *this; }
        const std::list<Property*>& operator=(const std::list<Property*>& value) { SetValue(value); return m_value; }

	// Property's virtual functions
	void SetHighlighted(bool value);
	void SetReadOnly(bool value);

	int GetPropertyType(){return ListOfPropertyType;}
	bool PropertyEditable() const {return false;}
	void CallSetFunction() { }
	void CallEditFunction() { }
	void CallSelectFunction() { }

	// MutableObject virtual functions - call thru to the owner object
	void AddProperty(Property *prop);	// Add to both the PropertyList and the Mutable list
	void OnPropertySet(Property *prop);
	void OnPropertyEdit(Property *prop);
	void OnPropertySelect(Property *prop);
	void OnPropertiesApply();
};

#endif
