
#if !defined Property_HEADER
#define Property_HEADER

#include "Binding.h"
#include "HeeksColor.h"
#include "DomainObject.h"
#include "Units.h"

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
    PropertyAxisType = 14,
    PropertyCoordType = 15
} PropertyType;

class TiXmlElement;


// Abstract base class for all Properties
//
class Property {
private:

    bool m_currently_binding;

protected:

    wxString m_name;
    wxString m_xmlname;
    wxString m_title;
    DomainObject * m_owner;
    bool m_bindable;
    Binding * m_binding;
    std::set<Binding *> m_subscriber_bindings;

    void (*readFromXmlFunc) ( Property * property, TiXmlElement *element );
    void (*writeToXmlFunc) ( const Property * property, TiXmlElement *element );

public:

    Property(const wxChar* name, const wxChar* title, DomainObject * owner = NULL);
    virtual ~Property();

    virtual int GetPropertyType() const {return PropertyInvalidType;}
    virtual const wxChar* GetPropertyTypeName() const = 0;
    virtual bool IsContainer() const {return false;}
    virtual bool IsHighlighted() = 0;
    virtual void SetHighlighted(bool value) = 0;
    virtual bool IsReadOnly() = 0;
    virtual void SetReadOnly(bool value) = 0;
    virtual bool IsVisible() = 0;
    virtual void SetVisible(bool value) = 0;
    virtual bool IsTransient() = 0;
    virtual void SetTransient(bool value) = 0;
    virtual bool PropertyEditable() const = 0;
    virtual Property * Clone ( ) const = 0;
    virtual void CopyMetadata ( const Property& prop ) { }

    bool IsBindable ( ) const {
        return m_bindable;
    }

    void SetBindable ( bool bindable ) {
        m_bindable = bindable;
    }

    Binding * GetBinding ( ) const {
        return m_binding;
    }

    void SetBinding ( Binding * binding ) {
        if ( m_binding ) {
            delete m_binding;
        }
        m_binding = binding;
    }

    // Called by the Binding destructor
    void SetBindingDeleted ( ) {
        m_binding = NULL;
    }

    bool HasSubscriberBindings ( ) const {
        return !m_subscriber_bindings.empty();
    }
    const std::set<Binding *>& GetSubscriberBindings ( ) const {
        return m_subscriber_bindings;
    }

    void RemoveSubscriberBinding ( Binding * b );
    void AddSubscriberBinding ( Binding * b );
    void ImportBindings ( Property& prop );

    virtual bool CallPreSetFunction();
    virtual void CallSetFunction();
    virtual void CallEditFunction();

    virtual void SetReadFromXmlFunction ( void (*func) ( Property * property, TiXmlElement *element ) ) {
        readFromXmlFunc = func;
    }

    virtual void SetWriteToXmlFunction ( void (*func) ( const Property * property, TiXmlElement *element )  ) {
        writeToXmlFunc = func;
    }

    virtual void ReadFromXmlElement ( TiXmlElement *element );

    virtual void WriteToXmlElement ( TiXmlElement *element ) const;

    virtual void DefaultReadFromXmlElement ( TiXmlElement *element ) = 0;
    virtual void DefaultReadBindingFromXmlElement ( TiXmlElement *element );
    virtual void DefaultWriteToXmlElement ( TiXmlElement *element ) const = 0;
    virtual void DefaultWriteBindingToXmlElement ( TiXmlElement *element ) const;

    virtual void operator = ( const Property& prop ) = 0;
    virtual bool operator == ( const Property& prop ) const { return this == &prop; }

    void SetName(const wxChar * name) { m_name = name; SetXmlName(name); }
    const wxString& GetName() const { return m_name; }

protected:
    void SetXmlName(const wxChar * name);

public:
    const wxString& GetXmlName() const { return m_xmlname; }

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
    bool m_transient;

public:

    PropertyTmpl()
    : Property(_(""), _("")), m_highlighted(false), m_readonly(false), m_visible(false), m_transient(false) { };

    PropertyTmpl(const wxChar* name, const wxChar* title, DomainObject* owner = NULL)
    : Property(name, title)
    { Initialize(title, owner); }

    virtual ~PropertyTmpl() { }

    virtual void Initialize(const wxChar* title, DomainObject* owner = NULL, bool bindable = false) {
        if ( GetName ( ).IsEmpty ( ) ) {
            SetName ( title );
        }
        m_title = title;
        m_highlighted = false;
        m_readonly = false;
        m_visible = true;
        m_transient = false;
        if ( owner ) {
            owner->AddProperty ( this );
        }
        SetBindable ( bindable );
    }

    virtual void SetValue(const PropType& value) {
        if ( ! CallPreSetFunction() )
            return;
        m_value = value;
        CallSetFunction();
    }
    virtual operator const PropType&() const { return m_value; }
    virtual bool IsHighlighted() { return m_highlighted; }
    virtual void SetHighlighted(bool value) { m_highlighted = value; }
    virtual bool IsReadOnly() { return m_readonly; }
    virtual void SetReadOnly(bool value) { m_readonly = value; }
    virtual bool IsVisible() { return m_visible; }
    virtual void SetVisible(bool value) { m_visible = value; }
    virtual bool IsTransient() { return m_transient; }
    virtual void SetTransient(bool value) { m_transient = value; }
    virtual bool PropertyEditable() const { return (m_readonly == false); }
};


class PropertyCheck : public PropertyTmpl<bool>{
public:
    PropertyCheck() : PropertyTmpl<bool>() { m_value = false; }
    PropertyCheck(bool value) : PropertyTmpl<bool>() { m_value = value; }
    PropertyCheck(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<bool>(name, title, owner) { m_value = false; }
    const PropertyCheck& operator=(const PropertyCheck& value) { SetValue(value.m_value); return *this; }
    bool IsSet() const { return m_value; }
    int GetPropertyType() const {return PropertyCheckType;}
    const wxChar* GetPropertyTypeName() const { return _("Check"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyChoice : public PropertyTmpl<int>{
public:
    std::list< wxString > m_choices;	// 0 is the first
    PropertyChoice() : PropertyTmpl<int>() { m_value = 0; }
    PropertyChoice(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyChoice(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<int>(name, title, owner) { m_value = 0; }
    const PropertyChoice& operator=(const PropertyChoice& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) { SetValue(value); return m_value; }
    int GetPropertyType() const {return PropertyChoiceType;}
    const wxChar* GetPropertyTypeName() const { return _("Choice"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyColor : public PropertyTmpl<HeeksColor>{
public:
    PropertyColor() : PropertyTmpl<HeeksColor>() { }
    PropertyColor(const HeeksColor& value) : PropertyTmpl<HeeksColor>() { m_value = value; }
    PropertyColor(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<HeeksColor>(name, title, owner) { }
    const PropertyColor& operator=(const PropertyColor& value) { SetValue(value.m_value); return *this; }
    const HeeksColor& operator=(const HeeksColor& value){SetValue(value); return m_value;}
    long COLORREF_color() const { return m_value.COLORREF_color(); }
    int GetPropertyType() const {return PropertyColorType;}
    const wxChar* GetPropertyTypeName() const { return _("Color"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyDouble : public PropertyTmpl<double>{
public:
    PropertyDouble() : PropertyTmpl<double>() { m_value = 0.0; }
    PropertyDouble(double value) : PropertyTmpl<double>() { m_value = value; }
    PropertyDouble(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<double>(name, title, owner) { m_value = 0.0; }
    const PropertyDouble& operator=(const PropertyDouble& value) { SetValue(value.m_value); return *this; }
    double operator=(double value){SetValue(value); return m_value;}
    int GetPropertyType() const { return PropertyDoubleType;}
    const wxChar* GetPropertyTypeName() const { return _("Floating Point"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyLength : public PropertyDouble{
private:
    mutable EnumUnitType m_units;
    mutable double m_converted_value;

public:
    PropertyLength();
    PropertyLength(double initial_value);
    PropertyLength(const wxChar* name, const wxChar* title, DomainObject* owner);

    void SetUnits(EnumUnitType units) {
        if ( ! CallPreSetFunction() )
            return;
        m_value *= Length::Conversion ( m_units, units );
        m_units = units;
        CallSetFunction();
    }

    EnumUnitType GetUnits() const { return m_units; }

    double GetActualValue() const { return m_value; }

    // Property's virtual functions
    virtual operator const double&() const;

    double operator=(double value) { SetValue(value); return value; }

    void SetValue(const double& value);

    void SetActualValue(const double& value);

    void CopyMetadata ( const Property& prop );

    int GetPropertyType() const {return PropertyLengthType;}
    const wxChar* GetPropertyTypeName() const { return _("Length"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyInt : public PropertyTmpl<int>{
public:
    PropertyInt() : PropertyTmpl<int>() { m_value = 0; }
    PropertyInt(int value) : PropertyTmpl<int>() { m_value = value; }
    PropertyInt(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<int>(name, title, owner) { m_value = 0; }
    const PropertyInt& operator=(const PropertyInt& value) { SetValue(value.m_value); return *this; }
    int operator=(int value) {SetValue(value); return m_value;}
    int GetPropertyType() const {return PropertyIntType;}
    const wxChar* GetPropertyTypeName() const { return _("Integer"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
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
    int GetPropertyType() const {return PropertyStringType;}
    const wxChar* GetPropertyTypeName() const { return _("Text String"); }
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
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
    int GetPropertyType() const {return PropertyFileType;}
    const wxChar* GetPropertyTypeName() const { return _("File"); }
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
    void Transform (const gp_Trsf &t) {
        if ( ! CallPreSetFunction() )
            return;
        m_value.Transform(t);
        CallSetFunction();
    }
    const gp_Vec& AsVector() { return m_value; }

    gp_Dir operator=(const gp_Dir& value) {SetValue(value); return m_value;}
    const gp_Vec& operator=(const gp_Vec& value) {SetValue(value); return m_value;}

    gp_Dir Normalize() const { return gp_Dir(m_value); }
    double X(const bool in_drawing_units = false) const;
    double Y(const bool in_drawing_units = false) const;
    double Z(const bool in_drawing_units = false) const;
    gp_XYZ XYZ() { return m_value.XYZ(); }

    void Reverse() {
        if ( ! CallPreSetFunction() )
         return;
        m_value.Reverse();
        CallSetFunction();
    }

    int GetPropertyType() const {return PropertyVectorType;}
    const wxChar* GetPropertyTypeName() const { return _("Vector"); }
    bool IsContainer() const {return true;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};


class PropertyVertex : public PropertyTmpl<gp_Pnt>, public DomainObject {
protected:
    bool m_changing;
    PropertyLength m_x;
    PropertyLength m_y;
    PropertyLength m_z;

public:
    PropertyVertex();
    PropertyVertex(const gp_Pnt& value);
    PropertyVertex(const wxChar* name, const wxChar* title, DomainObject* owner);

    void SetValue(const gp_Pnt& value) {
        if ( ! CallPreSetFunction() )
            return;
        m_value = value;
        m_changing = true;        // Only call set once
        m_x.SetValue ( m_value.X() );
        m_y.SetValue ( m_value.Y() );
        m_z.SetValue ( m_value.Z() );
        m_changing = false;
        CallSetFunction();
    }

    const PropertyVertex& operator=(const PropertyVertex& value) {
        SetValue(value.m_value);
        return *this;
    }

    bool OnPrePropertySet(Property&);      // Assignment via the equals operator
    void OnPropertySet(Property&);      // Assignment via the equals operator
    void OnPropertyEdit(Property&);

    // Wrap some basic methods for convenience
    void Transform (const gp_Trsf &t) {
        if ( ! CallPreSetFunction() )
            return;
        m_value.Transform(t);
        m_changing = true;        // Only call set once
        m_x.SetValue ( m_value.X() );
        m_y.SetValue ( m_value.Y() );
        m_z.SetValue ( m_value.Z() );
        m_changing = false;
        CallSetFunction();
    }
    const gp_Pnt& AsPoint() { return m_value; };

    const gp_Pnt& operator=(const gp_Pnt& value) {
        SetValue(value);
        return m_value;
    }

    double X() const;
    double Y() const;
    double Z() const;

    const PropertyLength& XProp() const;
    const PropertyLength& YProp() const;
    const PropertyLength& ZProp() const;

    void SetX(double x);
    void SetY(double y);
    void SetZ(double z);

    void SetActualX(double x);
    void SetActualY(double y);
    void SetActualZ(double z);

    gp_XYZ XYZ() { return m_value.XYZ(); }
    bool IsEqual(const gp_Pnt& other, const double tolerance) const { return m_value.IsEqual(other, tolerance); }
    double Distance(const gp_Pnt& other) const { return m_value.Distance(other); }

    virtual bool xyOnly()const{return false;}

    int GetPropertyType() const {return PropertyVertexType;}
    const wxChar* GetPropertyTypeName() const { return _("Vertex"); }
    bool IsContainer() const {return true;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
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

    int GetPropertyType() const {return PropertyVertex2dType;}
    const wxChar* GetPropertyTypeName() const { return _("2D Vertex"); }
    bool IsContainer() const {return true;}
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

    int GetPropertyType() const {return PropertyTrsfType;}
    const wxChar* GetPropertyTypeName() const { return _("Transform"); }
    bool IsContainer() const {return true;}

    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};

class PropertyAxis : public PropertyTmpl<gp_Ax1>{
public:
    PropertyAxis() : PropertyTmpl<gp_Ax1>() { }
    PropertyAxis(const gp_Ax1& value) : PropertyTmpl<gp_Ax1>() { m_value = value; }
    PropertyAxis(const wxChar* name, const wxChar* title, DomainObject* owner) : PropertyTmpl<gp_Ax1>(name, title, owner) { }
    const PropertyAxis& operator=(const PropertyAxis& value) { SetValue(value.m_value); return *this; }

    gp_Ax1& operator=(const gp_Ax1& value){ SetValue(value); return m_value; }

    void SetDirection(const gp_Dir& V) { return m_value.SetDirection(V); }
    const gp_Dir& Direction() const { return m_value.Direction(); }
    void SetLocation(const gp_Pnt& P) { return m_value.SetLocation(P); }
    const gp_Pnt& Location() const { return m_value.Location(); }

    void Transform (const gp_Trsf &T) {
        if ( ! CallPreSetFunction() )
         return;
        m_value.Transform(T);
        CallSetFunction();
    }

    void Reverse() {
        if ( ! CallPreSetFunction() )
         return;
        m_value.Reverse();
        CallSetFunction();
    }

    int GetPropertyType() const {return PropertyAxisType;}
    const wxChar* GetPropertyTypeName() const { return _("Axis"); }
    bool IsContainer() const {return true;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
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

    void Transform (const gp_Trsf &T) {
        if ( ! CallPreSetFunction() )
         return;
        m_value.Transform(T);
        CallSetFunction();
    }

    int GetPropertyType() const {return PropertyCoordType;}
    const wxChar* GetPropertyTypeName() const { return _("Coordinate System"); }
    bool IsContainer() const {return true;}
    void operator = ( const Property& prop );
    Property * Clone ( ) const;
    void DefaultReadFromXmlElement ( TiXmlElement *element );
    void DefaultWriteToXmlElement ( TiXmlElement *element ) const;
};

#endif
