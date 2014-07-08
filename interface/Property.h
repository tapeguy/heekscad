// Property.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.


#if !defined Property_HEADER
#define Property_HEADER

#include "HeeksColor.h"
#include "MutableObject.h"

enum{
	InvalidPropertyType,
	StringPropertyType,
	DoublePropertyType,
	LengthPropertyType,
	IntPropertyType,
	VertexPropertyType,
	VectorPropertyType,
	ChoicePropertyType,
	ColorPropertyType,
	CheckPropertyType,
	ListOfPropertyType,
	TrsfPropertyType,
	FilePropertyType,
	CoordPropertyType
};

// Abstract base class for all Properties
//
class Property {
public:
	virtual ~Property() { }
        virtual int GetPropertyType(){return InvalidPropertyType;}
	virtual bool IsHighlighted() = 0;
	virtual void SetHighlighted(bool value) = 0;
	virtual bool IsReadOnly() = 0;
	virtual void SetReadOnly(bool value) = 0;
	virtual bool IsVisible() = 0;
	virtual void SetVisible(bool value) = 0;
	virtual bool PropertyEditable() const = 0;
	virtual void CallSetFunction() = 0;
	virtual void CallEditFunction() = 0;
	virtual void CallSelectFunction() = 0;
	virtual const wxChar* GetShortString(void)const{return _("Unknown Property");}
};


template <class PropType>
class PropertyTmpl : public Property {

protected:
	wxString m_title;
	MutableObject* m_object;
	PropType m_value;
	bool m_highlighted;
	bool m_readonly;
	bool m_visible;

public:
	PropertyTmpl()
          :m_title(_("")), m_object(NULL), m_highlighted(false), m_readonly(false), m_visible(false) { };

	PropertyTmpl(const wxChar* title, const PropType& initial_value, MutableObject* object)
	{ m_value = initial_value; Initialize(title, object); }

	virtual ~PropertyTmpl() { }

	virtual void Initialize(const wxChar* title, MutableObject* object) {
		m_title = title;
		m_object = object;
		m_highlighted = false;
		m_readonly = false;
		m_visible = true;
		if(m_object)	m_object->AddProperty(this);
	}

	virtual void SetValue(const PropType& value) { m_value = value; CallSetFunction(); }
	virtual operator const PropType&() const { return m_value; }
	virtual void SetTitle(const wxChar * title) { m_title = title; }
	virtual bool IsHighlighted() { return m_highlighted; }
	virtual void SetHighlighted(bool value) { m_highlighted = value; }
	virtual bool IsReadOnly() { return m_readonly; }
	virtual void SetReadOnly(bool value) { m_readonly = value; }
	virtual bool IsVisible() { return m_visible; }
	virtual void SetVisible(bool value) { m_visible = value; }
	virtual bool PropertyEditable() const { return (m_readonly == false); }
	virtual void CallSetFunction() { if(m_object) m_object->OnPropertySet(this); }
	virtual void CallEditFunction() { if(m_object) m_object->OnPropertyEdit(this); }
	virtual void CallSelectFunction() { if(m_object) m_object->OnPropertySelect(this); }
	virtual const wxChar* GetShortString() const { return m_title.c_str(); }
};


class PropertyCheck : public PropertyTmpl<bool>{
public:
    PropertyCheck() : PropertyTmpl<bool>() { }
    PropertyCheck(bool value) : PropertyTmpl<bool>() { m_value = value; }
    PropertyCheck(const wxChar* t, bool initial_value, MutableObject* object) : PropertyTmpl<bool>(t, initial_value, object) { }
    const PropertyCheck& operator=(const PropertyCheck& value) { SetValue(value.m_value); return *this; }
    bool IsSet() { return m_value; }
    int GetPropertyType(){return CheckPropertyType;}
};


class PropertyChoice : public PropertyTmpl<int>{
public:
    std::list< wxString > m_choices;	// 0 is the first
    PropertyChoice() : PropertyTmpl<int>() { }
    PropertyChoice(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyChoice(const wxChar* t, int initial_value, MutableObject* object) : PropertyTmpl<int>(t, initial_value, object) { }
    const PropertyChoice& operator=(const PropertyChoice& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) { SetValue(value); return m_value; }
    int GetPropertyType(){return ChoicePropertyType;}
};


class PropertyColor : public PropertyTmpl<HeeksColor>{
public:
    PropertyColor() : PropertyTmpl<HeeksColor>() { }
    PropertyColor(const HeeksColor& value) : PropertyTmpl<HeeksColor>() { m_value = value; }
    PropertyColor(const wxChar* t, const HeeksColor& initial_value, MutableObject* object) : PropertyTmpl<HeeksColor>(t, initial_value, object) { }
    const PropertyColor& operator=(const PropertyColor& value) { SetValue(value.m_value); return *this; }
    const HeeksColor& operator=(const HeeksColor& value){SetValue(value); return m_value;}
    long COLORREF_color() const { return m_value.COLORREF_color(); }
    int GetPropertyType(){return ColorPropertyType;}
};


class PropertyDouble : public PropertyTmpl<double>{
public:
    PropertyDouble() : PropertyTmpl<double>() { };
    PropertyDouble(double value) : PropertyTmpl<double>() { m_value = value; }
    PropertyDouble(const wxChar* t, double initial_value, MutableObject* object) : PropertyTmpl<double>(t, initial_value, object) { }
    const PropertyDouble& operator=(const PropertyDouble& value) { SetValue(value.m_value); return *this; }
    double operator=(double value){SetValue(value); return m_value;}
    int GetPropertyType(){return DoublePropertyType;}
};


class PropertyLength : public PropertyDouble{
private:
    mutable double adjusted_length;

public:
    PropertyLength();
    PropertyLength(double initial_value);
    PropertyLength(const wxChar* t, double initial_value, MutableObject* object);

    void SetValue(const double& value);
    double operator=(double new_value);
    operator const double&() const;

    // Property's virtual functions
    int GetPropertyType(){return LengthPropertyType;}
};


class PropertyInt : public PropertyTmpl<int>{
public:
    PropertyInt() : PropertyTmpl<int>() { }
    PropertyInt(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyInt(const wxChar* t, int initial_value, MutableObject* object) : PropertyTmpl<int>(t, initial_value, object) { }
    const PropertyInt& operator=(const PropertyInt& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) {SetValue(value); return m_value;}
    int GetPropertyType(){return IntPropertyType;}
};


class PropertyString : public PropertyTmpl<wxString>{
public:
    PropertyString() : PropertyTmpl<wxString>() { }
    PropertyString(const wxChar* value) : PropertyTmpl<wxString>() { m_value = value; }
    PropertyString(const wxChar* t, const wxChar* initial_value, MutableObject* object) : PropertyTmpl<wxString>(t, initial_value, object) { }
    const PropertyString& operator=(const PropertyString& value) { SetValue(value.m_value); return *this; }

    operator const wxChar *() const {return m_value;}
    const wxString& operator=(const wxString& value) {SetValue(value); return m_value;}
    const wxString& operator=(const wxChar* value) {SetValue(value); return m_value;}

    int GetPropertyType() {return StringPropertyType;}
};


class PropertyFile : public PropertyString{
public:
    PropertyFile() : PropertyString() { }
    PropertyFile(const wxChar* value) : PropertyString(value) { }
    PropertyFile(const wxChar* t, const wxChar* initial_value, MutableObject* object) : PropertyString(t, initial_value, object) { }
    const PropertyFile& operator=(const PropertyFile& value) { SetValue(value.m_value); return *this; }

    operator const wxChar *() const {return m_value;}
    const wxString& operator=(wxString value) {SetValue(value); return m_value;}
    const wxString& operator=(const wxChar* value) {SetValue(value); return m_value;}

    int GetPropertyType(){return FilePropertyType;}
};


class PropertyVector : public PropertyTmpl<gp_Vec> {
public:
    PropertyVector() : PropertyTmpl<gp_Vec>() { }
    PropertyVector(const gp_Vec& value) : PropertyTmpl<gp_Vec>() { m_value = value; }
    PropertyVector(const wxChar* t, const gp_Vec& initial_value, MutableObject* object) : PropertyTmpl<gp_Vec>(t, initial_value, object) { }
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

    int GetPropertyType(){return VectorPropertyType;}
};


class PropertyVertex : public PropertyTmpl<gp_Pnt> {
public:
    PropertyVertex() : PropertyTmpl<gp_Pnt>() { };
    PropertyVertex(const gp_Pnt& value) : PropertyTmpl<gp_Pnt>() { m_value = value; }
    PropertyVertex(const wxChar *t, const gp_Pnt& initial_vt, MutableObject* object) : PropertyTmpl<gp_Pnt>(t, initial_vt, object) { }
    PropertyVertex(const wxChar *t, const double *initial_vt, MutableObject* object) : PropertyTmpl<gp_Pnt>(t, gp_Pnt(initial_vt[0], initial_vt[1], initial_vt[2]), object) { }
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

    int GetPropertyType(){return VertexPropertyType;}

    virtual bool xyOnly()const{return false;}
};


class PropertyVertex2d : public PropertyVertex{
public:
    PropertyVertex2d() : PropertyVertex() { }
    PropertyVertex2d(const gp_Pnt& value) : PropertyVertex(value) { }
    PropertyVertex2d(const wxChar *t, const gp_Pnt& initial_vt, MutableObject* object) : PropertyVertex(t, initial_vt, object) { }
    PropertyVertex2d(const wxChar *t, const double *initial_vt, MutableObject* object) : PropertyVertex(t, initial_vt, object) { }
    const PropertyVertex2d& operator=(const PropertyVertex2d& value) { SetValue(value.m_value); return *this; }

    const gp_Pnt& operator=(const gp_Pnt& value){ SetValue(value); return m_value; }

    // PropertyVertex's virtual functions
    bool xyOnly()const{return true;}
};


class PropertyTrsf : public PropertyTmpl<gp_Trsf>{
public:
    PropertyTrsf() : PropertyTmpl<gp_Trsf>() { }
    PropertyTrsf(const gp_Trsf& value) : PropertyTmpl<gp_Trsf>() { m_value = value; }
    PropertyTrsf(const wxChar* t, const gp_Trsf& initial_value, MutableObject* object) : PropertyTmpl<gp_Trsf>(t, initial_value, object) { }
    const PropertyTrsf& operator=(const PropertyTrsf& value) { SetValue(value.m_value); return *this; }

    gp_Trsf& operator=(const gp_Trsf& value){ SetValue(value); return m_value; }

    int GetPropertyType(){return TrsfPropertyType;}
};

class PropertyCoord : public PropertyTmpl<gp_Ax2>{
public:
    PropertyCoord() : PropertyTmpl<gp_Ax2>() { }
    PropertyCoord(const gp_Ax2& value) : PropertyTmpl<gp_Ax2>() { m_value = value; }
    PropertyCoord(const wxChar* t, const gp_Ax2& initial_value, MutableObject* object) : PropertyTmpl<gp_Ax2>(t, initial_value, object) { }
    const PropertyCoord& operator=(const PropertyCoord& value) { SetValue(value.m_value); return *this; }

    gp_Ax2& operator=(const gp_Ax2& value){ SetValue(value); return m_value; }

    const gp_Dir& Direction() const { return m_value.Direction(); }
    const gp_Dir& XDirection() const { return m_value.XDirection(); }
    const gp_Dir& YDirection() const { return m_value.YDirection(); }
    const gp_Pnt& Location() const { return m_value.Location(); }
    void Transform (const gp_Trsf &T) { return m_value.Transform(T); }

    int GetPropertyType(){return CoordPropertyType;}
};

#endif
