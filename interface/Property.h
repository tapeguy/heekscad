// Property.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.


#if !defined Property_HEADER
#define Property_HEADER

#include "HeeksColor.h"
#include "DomainObject.h"


typedef enum
{
    PropertyInvalidType = 0,
    PropertyStringType = 1,
    PropertyDoubleType = 2,
    PropertyLengthType = 3,
    PropertyIntType = 4,
    PropertyVertexType = 5,
    PropertyVertex2dType = 6,
    PropertyVectorType = 7,
    PropertyChoiceType = 8,
    PropertyColorType = 9,
    PropertyCheckType = 10,
    PropertyListType = 11,
    PropertyTrsfType = 12,
    PropertyFileType = 13,
    PropertyCoordType = 14
} PropertyType;

// Abstract base class for all Properties
//
class Property {
protected:

    wxString m_name;
    wxString m_title;
    DomainObject * m_owner;

public:

    Property(const wxChar* name, const wxChar* title, DomainObject * owner = NULL)
    : m_name(name), m_title(title), m_owner(owner)
    { }
	virtual ~Property() { }
	virtual int GetPropertyType(){return PropertyInvalidType;}
	virtual bool IsHighlighted() = 0;
	virtual void SetHighlighted(bool value) = 0;
	virtual bool IsReadOnly() = 0;
	virtual void SetReadOnly(bool value) = 0;
	virtual bool IsVisible() = 0;
	virtual void SetVisible(bool value) = 0;
	virtual bool PropertyEditable() const = 0;
    virtual void CallEditFunction() = 0;
    virtual Property * Clone ( ) const = 0;

    virtual bool operator == ( const Property& prop ) const { return this == &prop; }

    void SetName(const wxChar * name) { m_name = name; }
    const wxString& GetName() const { return m_name; }

    void SetTitle(const wxChar * title) { m_title = title; }
    const wxString& GetTitle() const { return m_title; }

    void SetOwner(DomainObject * owner) { m_owner = owner; }
    DomainObject * GetOwner() const { return m_owner; }

    virtual const wxChar* GetShortString() const { return m_title.c_str(); }

    friend class DomainObject;
};


template <class PropType>
class PropertyTmpl : public Property {
protected:

	PropType m_value;
	bool m_highlighted;
	bool m_readonly;
	bool m_visible;

public:

	PropertyTmpl()
    : Property(_(""), _("")), m_highlighted(false), m_readonly(false), m_visible(false) { };

	PropertyTmpl(const wxChar* name, const wxChar* title, DomainObject* owner = NULL)
	: Property(name, title)
	{ Initialize(title, owner); }

	virtual ~PropertyTmpl() { }

	virtual void Initialize(const wxChar* title, DomainObject* owner = NULL) {
	    if (GetName ( ).IsEmpty()) {
	        SetName ( title );
	    }
	    m_title = title;
		m_highlighted = false;
		m_readonly = false;
		m_visible = true;
		if(owner) {
		    owner->AddProperty(this);
		}
	}

	virtual void SetValue(const PropType& value) { m_value = value; CallSetFunction(); }
	virtual operator const PropType&() const { return m_value; }
	virtual bool IsHighlighted() { return m_highlighted; }
	virtual void SetHighlighted(bool value) { m_highlighted = value; }
	virtual bool IsReadOnly() { return m_readonly; }
	virtual void SetReadOnly(bool value) { m_readonly = value; }
	virtual bool IsVisible() { return m_visible; }
	virtual void SetVisible(bool value) { m_visible = value; }
	virtual bool PropertyEditable() const { return (m_readonly == false); }
	virtual void CallSetFunction()
	{
	    DomainObject * owner = (DomainObject *)this->GetOwner();
	        if (owner)
	            owner->OnPropertySet(*this);
	}

	virtual void CallEditFunction()
    {
	    DomainObject * owner = (DomainObject *)this->GetOwner();
            if (owner)
                owner->OnPropertyEdit(*this);
    }
};


class PropertyCheck : public PropertyTmpl<bool>{
public:
    PropertyCheck() : PropertyTmpl<bool>() { }
    PropertyCheck(bool value) : PropertyTmpl<bool>() { m_value = value; }
    PropertyCheck(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<bool>(name, title, owner) { }
    const PropertyCheck& operator=(const PropertyCheck& value) { SetValue(value.m_value); return *this; }
    bool IsSet() const { return m_value; }
    int GetPropertyType(){return PropertyCheckType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyChoice : public PropertyTmpl<int>{
public:
    std::list< wxString > m_choices;	// 0 is the first
    PropertyChoice() : PropertyTmpl<int>() { }
    PropertyChoice(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyChoice(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<int>(name, title, owner) { }
    const PropertyChoice& operator=(const PropertyChoice& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) { SetValue(value); return m_value; }
    int GetPropertyType(){return PropertyChoiceType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyColor : public PropertyTmpl<HeeksColor>{
public:
    PropertyColor() : PropertyTmpl<HeeksColor>() { }
    PropertyColor(const HeeksColor& value) : PropertyTmpl<HeeksColor>() { m_value = value; }
    PropertyColor(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<HeeksColor>(name, title, owner) { }
    const PropertyColor& operator=(const PropertyColor& value) { SetValue(value.m_value); return *this; }
    const HeeksColor& operator=(const HeeksColor& value){SetValue(value); return m_value;}
    long COLORREF_color() const { return m_value.COLORREF_color(); }
    int GetPropertyType(){return PropertyColorType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyDouble : public PropertyTmpl<double>{
public:
    PropertyDouble() : PropertyTmpl<double>() { };
    PropertyDouble(double value) : PropertyTmpl<double>() { m_value = value; }
    PropertyDouble(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<double>(name, title, owner) { }
    const PropertyDouble& operator=(const PropertyDouble& value) { SetValue(value.m_value); return *this; }
    double operator=(double value){SetValue(value); return m_value;}
    int GetPropertyType(){return PropertyDoubleType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyLength : public PropertyDouble{
private:
    mutable double adjusted_length;

public:
    PropertyLength();
    PropertyLength(double initial_value);
    PropertyLength(const wxChar* name, const wxChar* title, DomainObject* owner);

    void SetValue(const double& value);
    double operator=(double new_value);
    operator const double&() const;

    // Property's virtual functions
    int GetPropertyType(){return PropertyLengthType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyInt : public PropertyTmpl<int>{
public:
    PropertyInt() : PropertyTmpl<int>() { }
    PropertyInt(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyInt(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<int>(name, title, owner) { }
    const PropertyInt& operator=(const PropertyInt& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) {SetValue(value); return m_value;}
    int GetPropertyType(){return PropertyIntType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyString : public PropertyTmpl<wxString>{
public:
    PropertyString() : PropertyTmpl<wxString>() { }
    PropertyString(const wxChar* value) : PropertyTmpl<wxString>() { m_value = value; }
    PropertyString(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<wxString>(name, title, owner) { }
    const PropertyString& operator=(const PropertyString& value) { SetValue(value.m_value); return *this; }
    operator const wxChar *() const {return m_value;}
    const wxString& operator=(const wxString& value) {SetValue(value); return m_value;}
    const wxString& operator=(const wxChar* value) {SetValue(value); return m_value;}
    int GetPropertyType() {return PropertyStringType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyFile : public PropertyString{
public:
    PropertyFile() : PropertyString() { }
    PropertyFile(const wxChar* value) : PropertyString(value) { }
    PropertyFile(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyString(name, title, owner) { }
    const PropertyFile& operator=(const PropertyFile& value) { SetValue(value.m_value); return *this; }
    operator const wxChar *() const {return m_value;}
    const wxString& operator=(wxString value) {SetValue(value); return m_value;}
    const wxString& operator=(const wxChar* value) {SetValue(value); return m_value;}
    int GetPropertyType(){return PropertyFileType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyVector : public PropertyTmpl<gp_Vec> {
public:
    PropertyVector() : PropertyTmpl<gp_Vec>() { }
    PropertyVector(const gp_Vec& value) : PropertyTmpl<gp_Vec>() { m_value = value; }
    PropertyVector(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<gp_Vec>(name, title, owner) { }
    const PropertyVector& operator=(const PropertyVector& value) { SetValue(value.m_value); return *this; }

    // Wrap some basic methods for convenience
    void Transform (const gp_Trsf &t) { m_value.Transform(t); CallSetFunction(); }
    const gp_Vec& AsVector() { return m_value; }

    gp_Dir operator=(const gp_Dir& value) {SetValue(value); return m_value;}
    const gp_Vec& operator=(const gp_Vec& value) {SetValue(value); return m_value;}

    gp_Dir Normalize() { return gp_Dir(m_value); }
    double X(const bool in_drawing_units = false) const;
    double Y(const bool in_drawing_units = false) const;
    double Z(const bool in_drawing_units = false) const;
    gp_XYZ XYZ() { return m_value.XYZ(); }

    int GetPropertyType(){return PropertyVectorType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyVertex : public PropertyTmpl<gp_Pnt> {
public:
    PropertyVertex() : PropertyTmpl<gp_Pnt>() { };
    PropertyVertex(const gp_Pnt& value) : PropertyTmpl<gp_Pnt>() { m_value = value; }
    PropertyVertex(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<gp_Pnt>(name, title, owner) { }
    const PropertyVertex& operator=(const PropertyVertex& value) { SetValue(value.m_value); return *this; }

    // Wrap some basic methods for convenience
    void Transform (const gp_Trsf &t) { m_value.Transform(t); CallSetFunction(); }
    const gp_Pnt& AsPoint() { return m_value; };

    const gp_Pnt& operator=(const gp_Pnt& value) { SetValue(value); return m_value; }

    double X(const bool in_drawing_units = false) const;
    double Y(const bool in_drawing_units = false) const;
    double Z(const bool in_drawing_units = false) const;
    void SetX(double x) { m_value.SetX(x); CallSetFunction(); }
    void SetY(double y) { m_value.SetY(y); CallSetFunction(); }
    void SetZ(double z) { m_value.SetZ(z); CallSetFunction(); }
    gp_XYZ XYZ() { return m_value.XYZ(); }
    bool IsEqual(const gp_Pnt& other, const double tolerance) { return m_value.IsEqual(other, tolerance); }
    double Distance(const gp_Pnt& other) { return m_value.Distance(other); }

    virtual bool xyOnly()const{return false;}

    int GetPropertyType(){return PropertyVertexType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyVertex2d : public PropertyVertex{
public:
    PropertyVertex2d() : PropertyVertex() { }
    PropertyVertex2d(const gp_Pnt& value) : PropertyVertex(value) { }
    PropertyVertex2d(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyVertex(name, title, owner) { }
    const PropertyVertex2d& operator=(const PropertyVertex2d& value) { SetValue(value.m_value); return *this; }

    const gp_Pnt& operator=(const gp_Pnt& value){ SetValue(value); return m_value; }

    // PropertyVertex's virtual functions
    bool xyOnly()const{return true;}

    int GetPropertyType(){return PropertyVertex2dType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};


class PropertyTrsf : public PropertyTmpl<gp_Trsf>{
public:
    PropertyTrsf() : PropertyTmpl<gp_Trsf>() { }
    PropertyTrsf(const gp_Trsf& value) : PropertyTmpl<gp_Trsf>() { m_value = value; }
    PropertyTrsf(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<gp_Trsf>(name, title, owner) { }
    const PropertyTrsf& operator=(const PropertyTrsf& value) { SetValue(value.m_value); return *this; }

    gp_Trsf& operator=(const gp_Trsf& value){ SetValue(value); return m_value; }

    int GetPropertyType(){return PropertyTrsfType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};

class PropertyCoord : public PropertyTmpl<gp_Ax2>{
public:
    PropertyCoord() : PropertyTmpl<gp_Ax2>() { }
    PropertyCoord(const gp_Ax2& value) : PropertyTmpl<gp_Ax2>() { m_value = value; }
    PropertyCoord(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<gp_Ax2>(name, title, owner) { }
    const PropertyCoord& operator=(const PropertyCoord& value) { SetValue(value.m_value); return *this; }

    gp_Ax2& operator=(const gp_Ax2& value){ SetValue(value); return m_value; }

    const gp_Dir& Direction() const { return m_value.Direction(); }
    const gp_Dir& XDirection() const { return m_value.XDirection(); }
    const gp_Dir& YDirection() const { return m_value.YDirection(); }
    const gp_Pnt& Location() const { return m_value.Location(); }
    void Transform (const gp_Trsf &T) { return m_value.Transform(T); }

    int GetPropertyType(){return PropertyCoordType;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
};

#endif
