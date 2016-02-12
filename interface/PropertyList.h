// PropertyList.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#if !defined PropertyList_HEADER
#define PropertyList_HEADER

#include "Property.h"

class PropertyList : public PropertyTmpl<std::list< Property* > >, public DomainObject
{
public:
	PropertyList();
	PropertyList(const wxChar* name, const wxChar* title, DomainObject* owner);

    const std::list<Property*>& operator=(const std::list<Property*>& value) { SetValue(value); return m_value; }

	// Property's virtual functions
	void SetHighlighted(bool value);
	void SetReadOnly(bool value);

	int GetPropertyType() const {return PropertyListType;}
    const wxChar* GetPropertyTypeName() const { return _("List"); }
    bool IsTypeBindable() const {return false;}
    bool IsContainer() const {return true;}
	bool PropertyEditable() const {return false;}
	void CallSetFunction() { }

	// DomainObject virtual functions - call thru to the owner object
	bool AddProperty(Property *prop);	// Add to both the PropertyList and the DomainObject list
	void RemoveProperty(Property *prop);
	bool OnPrePropertySet(Property&);      // Assignment via the equals operator
	void OnPropertySet(Property&);      // Assignment via the equals operator
	void OnPropertyEdit(Property&);
	void Clear();

    void operator = ( const Property& prop );
    Property * Clone ( ) const;

    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};

#endif
