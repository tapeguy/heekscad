// HeeksObj.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

class Property;

// Abstract interface for objects that have changable properties.
//
class MutableObject {

private:
	std::list<Property*> m_properties;

public:
	MutableObject() { }
	virtual ~MutableObject() { }

	virtual void GetProperties(std::list<Property*> *list) { list->insert(list->end(), m_properties.begin(), m_properties.end()); }
	virtual bool ValidateProperties() { return true; }
	virtual void InitializeProperties() { }

	// Override these callbacks:
	virtual void OnPropertySet(Property *) { }      // Assignment via the equals operator
	virtual void OnPropertyEdit(Property *) { }     // User edit in the GUI
	virtual void OnPropertySelect(Property *) { }   // User select in the GUI
	virtual void OnPropertiesApply(Property *) { }  // Apply button in GUI
};
