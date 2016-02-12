// Property.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"

#include "Property.h"

Property::Property(const wxChar* name, const wxChar* title, DomainObject * owner)
: m_name(name), m_title(title), m_owner(owner), m_bindable(false), m_binding(NULL)
{
    SetXmlName(name);
    readFromXmlFunc = NULL;
    writeToXmlFunc = NULL;
    m_currently_binding = false;
}

Property::~Property()
{
    Binding* del = m_binding;
    std::set<Binding *>::iterator it;
    if (m_binding) {

        m_binding = NULL;   // Set to null to prevent re-invoking the binding destructor.
        del->RemoveProperty(this);
        delete del;
    }
    for (it = m_subscriber_bindings.begin(); it != m_subscriber_bindings.end(); it++) {
        Binding* binding = *it;
        if ( binding != del ) {
            binding->RemoveProperty(this);
            delete binding;
        }
    }
}

void Property::RemoveSubscriberBinding ( Binding * b )
{
    std::set<Binding *>::iterator it = m_subscriber_bindings.find ( b );
    if (it != m_subscriber_bindings.end()) {
        m_subscriber_bindings.erase ( it );
    }
}

void Property::AddSubscriberBinding ( Binding * b )
{
    m_subscriber_bindings.insert ( b );
}

void Property::ImportBindings ( Property& prop )
{
    Binding * binding;
    binding = prop.GetBinding ( );
    this->SetBinding ( binding );
    if ( binding ) {
        binding->ReplaceProperty ( &prop, this );
    }
    prop.SetBindingDeleted ( );
    std::set<Binding *> bset = prop.GetSubscriberBindings ( );
    std::set<Binding *>::iterator it;
    for ( it = bset.begin ( ); it != bset.end ( ); it++ )
    {
        binding = *it;
        binding->ReplaceProperty ( &prop, this );
        AddSubscriberBinding ( *it );
    }
    prop.m_subscriber_bindings.clear ( );
}

bool Property::CallPreSetFunction()
{
    DomainObject * owner = this->GetOwner ( );
    if ( owner )
        return owner->OnPrePropertySet ( *this );
    return true;
}

void Property::CallSetFunction()
{
    if ( m_currently_binding ) {
        return;
    }

    m_currently_binding = true;
    std::set<Binding *>::iterator it;
    for (it = m_subscriber_bindings.begin(); it != m_subscriber_bindings.end(); it++) {
        Binding* b = *it;
        b->Execute(this);
    }

    // Notify the owner after the bindings execute; the owner may swap out the object.
    DomainObject * owner = this->GetOwner();
    if (owner) {
        owner->OnPropertySet(*this);
    }

    m_currently_binding = false;
}

void Property::CallEditFunction()
{
    DomainObject * owner = this->GetOwner();
    if (owner)
        owner->OnPropertyEdit(*this);
}

void Property::ReadFromXmlElement ( TiXmlElement *element )
{
    if (readFromXmlFunc)
        (*readFromXmlFunc) ( this, element );
    else
        DefaultReadFromXmlElement ( element );
}

void Property::WriteToXmlElement ( TiXmlElement *element ) const
{
    if (writeToXmlFunc)
        (*writeToXmlFunc) ( this, element );
    else
        DefaultWriteToXmlElement ( element );
}

void Property::SetXmlName(const wxChar * name)
{
    m_xmlname = name;
    m_xmlname.Replace( _(" "), _("_") );
    m_xmlname.Replace( _("/"), _("") );
    const char *invalid_chars = "!@#$%^&*()+=/\[]{}|";
    size_t found = m_xmlname.find_first_of(invalid_chars);
    while ( found != wxString::npos ) {
        m_xmlname.erase(found, 1);
        found = m_xmlname.find_first_of(invalid_chars, found);
    }
    m_xmlname.MakeLower();
}

void PropertyCheck::operator = ( const Property& prop )
{
    const PropertyCheck * value = &(const PropertyCheck&)prop;
    SetValue(value->m_value);
}

Property * PropertyCheck::Clone ( ) const
{
    PropertyCheck * rtn = new PropertyCheck(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyCheck::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    wxString text = element->GetText();
    SetValue ( text == "true" ? true : false );
}

void PropertyCheck::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    prop_element->LinkEndChild ( new TiXmlText ( m_value ? "true" : "false" ) );
}

void PropertyChoice::operator = ( const Property& prop )
{
    const PropertyChoice * value = &(const PropertyChoice&)prop;
    SetValue(value->m_value);
}

Property * PropertyChoice::Clone ( ) const
{
    PropertyChoice * rtn = new PropertyChoice(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyChoice::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    wxString text = element->GetText();
    long value;
    if ( text.ToLong ( &value ) )
        SetValue ( value );
}

void PropertyChoice::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    wxString text;
    text << m_value;
    prop_element->LinkEndChild ( new TiXmlText ( text ) );
}

void PropertyColor::operator = ( const Property& prop )
{
    const PropertyColor * value = &(const PropertyColor&)prop;
    SetValue(value->m_value);
}

Property * PropertyColor::Clone ( ) const
{
    PropertyColor * rtn = new PropertyColor(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyColor::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    wxString text = element->GetText();

    m_value.FromHtmlColor(text);
    CallSetFunction();
}

void PropertyColor::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    prop_element->LinkEndChild ( new TiXmlText ( m_value.ToHtmlColor() ) );
}

void PropertyDouble::operator = ( const Property& prop )
{
    const PropertyDouble * value = &(const PropertyDouble&)prop;
    SetValue(value->m_value);
}

Property * PropertyDouble::Clone ( ) const
{
    PropertyDouble * rtn = new PropertyDouble(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

static double GetDoubleFromXmlElement ( TiXmlElement * element, const char * name )
{
    TiXmlElement* prop_element = element->FirstChildElement(name);
    wxString text = prop_element->GetText();
    double d = 0.0;
    text.ToDouble(&d);
    return d;
}

static TiXmlElement* AddDoubleToXmlElement ( TiXmlElement * element, const char * name, double value )
{
    TiXmlElement* prop_element = new TiXmlElement ( name );
    element->LinkEndChild ( prop_element );

    std::ostringstream ss;
    ss.imbue(std::locale("C"));
    ss.precision(prop_element->Precision(value));
    ss << value;

    prop_element->LinkEndChild ( new TiXmlText ( ss.str() ) );
    return prop_element;
}

void PropertyDouble::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    wxString text = element->GetText();
    double d;
    text.ToDouble(&d);
    SetValue(d);
}

void PropertyDouble::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    AddDoubleToXmlElement ( element, this->GetXmlName(), m_value );
}

PropertyLength::PropertyLength()
:PropertyDouble(), m_converted_value(0.0)
{
    m_units = wxGetApp().GetViewUnits();
}

PropertyLength::PropertyLength(double initial_value)
:PropertyDouble()
{
    m_converted_value = initial_value;
	m_value = initial_value;
	m_units = wxGetApp().GetViewUnits();
}

PropertyLength::PropertyLength(const wxChar* name, const wxChar* title, DomainObject* owner)
:PropertyDouble(name, title, owner), m_converted_value(0.0)
{
    m_units = wxGetApp().GetViewUnits();
}

PropertyLength::operator const double&() const
{
    m_converted_value = m_value * Length::Conversion ( m_units, wxGetApp().GetViewUnits ( ) );
    return m_converted_value;
}

void PropertyLength::SetValue(const double& value)
{
    if ( ! CallPreSetFunction() )
        return;
    m_value = value * Length::Conversion ( wxGetApp().GetViewUnits ( ), m_units );
    CallSetFunction();
}

void PropertyLength::SetActualValue(const double& value)
{
    if ( ! CallPreSetFunction() )
        return;
    m_value = value;
    CallSetFunction();
}

void PropertyLength::CopyMetadata ( const Property& prop )
{
    const PropertyLength * other = &(const PropertyLength&) prop;
    if ( m_units != other->m_units )
    {
        m_value = m_value * Length::Conversion ( m_units, other->m_units );
        m_units = other->m_units;
    }
}

void PropertyLength::operator = ( const Property& prop )
{
    const PropertyLength * other = &(const PropertyLength&) prop;
    m_units = other->m_units;
    SetActualValue ( other->m_value );
}

Property * PropertyLength::Clone ( ) const
{
    PropertyLength * rtn = new PropertyLength(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->m_units = m_units;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyLength::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    const char * units = element->Attribute("units");
    if (units)
    {
        wxString units_str ( units );
        this->m_units = GetEnumUnitTypeFromString ( units_str );
    }
    wxString text = element->GetText();
    double d;
    text.ToDouble(&d);
    SetValue(d);
}

void PropertyLength::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    wxString units = ::GetShortString(this->m_units);
    TiXmlElement * new_element = AddDoubleToXmlElement ( element, this->GetXmlName(), m_value );
    new_element->SetAttribute("units", units);
}

void PropertyInt::operator = ( const Property& prop )
{
    const PropertyInt * value = &(const PropertyInt&)prop;
    SetValue(value->m_value);
}

Property * PropertyInt::Clone ( ) const
{
    PropertyInt * rtn = new PropertyInt(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyInt::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    wxString text = element->GetText();
    long int i;
    text.ToLong(&i);
    SetValue(i);
}

void PropertyInt::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    wxString text;
    text << m_value;
    prop_element->LinkEndChild ( new TiXmlText ( text ) );
}

void PropertyString::operator = ( const Property& prop )
{
    const PropertyString * value = &(const PropertyString&)prop;
    SetValue(value->m_value);
}

Property * PropertyString::Clone ( ) const
{
    PropertyString * rtn = new PropertyString(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyString::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    SetValue(element->GetText());
}

void PropertyString::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    prop_element->LinkEndChild ( new TiXmlText ( m_value ) );
}

void PropertyFile::operator = ( const Property& prop )
{
    const PropertyFile * value = &(const PropertyFile&)prop;
    SetValue(value->m_value);
}

Property * PropertyFile::Clone ( ) const
{
    PropertyFile * rtn = new PropertyFile(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

double PropertyVector::X(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.X() / wxGetApp().GetViewUnits() : m_value.X();
}

double PropertyVector::Y(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Y() / wxGetApp().GetViewUnits() : m_value.Y();
}

double PropertyVector::Z(const bool in_drawing_units /* = false */) const
{
	return in_drawing_units ? m_value.Z() / wxGetApp().GetViewUnits() : m_value.Z();
}

void PropertyVector::operator = ( const Property& prop )
{
    const PropertyVector * value = &(const PropertyVector&)prop;
    SetValue(value->m_value);
}

Property * PropertyVector::Clone ( ) const
{
    PropertyVector * rtn = new PropertyVector(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyVector::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    m_value.SetX ( GetDoubleFromXmlElement (element, "x") );
    m_value.SetY ( GetDoubleFromXmlElement (element, "y") );
    m_value.SetZ ( GetDoubleFromXmlElement (element, "z") );
    CallSetFunction();
}

void PropertyVector::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    AddDoubleToXmlElement ( prop_element, "x", m_value.X() );
    AddDoubleToXmlElement ( prop_element, "y", m_value.Y() );
    AddDoubleToXmlElement ( prop_element, "z", m_value.Z() );
}

PropertyVertex::PropertyVertex()
 : PropertyTmpl<gp_Pnt>(),
   m_changing(false), m_x(_("x"), _("x"), this), m_y(_("y"), _("y"), this), m_z(_("z"), _("z"), this)
{
}

PropertyVertex::PropertyVertex(const gp_Pnt& value)
 : PropertyTmpl<gp_Pnt>(),
   m_changing(false), m_x(_("x"), _("x"), NULL), m_y(_("y"), _("y"), NULL), m_z(_("z"), _("z"), NULL)
{
    m_value = value;
    m_x = m_value.X();
    m_y = m_value.Y();
    m_z = m_value.Z();
    m_x.SetOwner(this);
    m_y.SetOwner(this);
    m_z.SetOwner(this);
}

PropertyVertex::PropertyVertex(const wxChar* name, const wxChar* title, DomainObject* owner)
 : PropertyTmpl<gp_Pnt>(name, title, owner),
   m_changing(false), m_x(_("x"), _("x"), this), m_y(_("y"), _("y"), this), m_z(_("z"), _("z"), this)
{
}

bool PropertyVertex::OnPrePropertySet(Property& prop)
{
    if (m_changing) {
        return TRUE;
    }
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPrePropertySet(prop);
    return TRUE;
}

void PropertyVertex::OnPropertySet(Property& prop)
{
    if (m_changing) {
        return;
    }
    DomainObject * owner = this->GetOwner();
    if (owner)
        owner->OnPropertySet(prop);
}

void PropertyVertex::OnPropertyEdit(Property& prop)
{
    if (m_changing) {
        return;
    }
    DomainObject * owner = this->GetOwner();
    if (owner)
        return owner->OnPropertyEdit(prop);
}

double PropertyVertex::X() const
{
	return m_value.X();
}

double PropertyVertex::Y() const
{
	return m_value.Y();
}

double PropertyVertex::Z() const
{
	return m_value.Z();
}

const PropertyLength& PropertyVertex::XProp() const
{
    return m_x;
}

const PropertyLength& PropertyVertex::YProp() const
{
    return m_y;
}

const PropertyLength& PropertyVertex::ZProp() const
{
    return m_z;
}

void PropertyVertex::SetX(double x) {
    m_x.SetValue(x);
    m_value.SetX(x);
}

void PropertyVertex::SetY(double y) {
    m_y.SetValue(y);
    m_value.SetY(y);
}

void PropertyVertex::SetZ(double z) {
    m_z.SetValue(z);
    m_value.SetZ(z);
}

void PropertyVertex::SetActualX(double x) {
    m_x.SetActualValue(x);
    m_value.SetX(x);
}

void PropertyVertex::SetActualY(double y) {
    m_y.SetActualValue(y);
    m_value.SetY(y);
}

void PropertyVertex::SetActualZ(double z) {
    m_z.SetActualValue(z);
    m_value.SetZ(z);
}

void PropertyVertex::operator = ( const Property& prop )
{
    const PropertyVertex * value = &(const PropertyVertex&)prop;
    SetValue(value->m_value);
}

Property * PropertyVertex::Clone ( ) const
{
    PropertyVertex * rtn = new PropertyVertex(m_name, m_title, NULL);
    rtn->m_value = m_value;
    rtn->m_x = m_x;
    rtn->m_y = m_y;
    rtn->m_z = m_z;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyVertex::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    m_value.SetX ( GetDoubleFromXmlElement (element, "x") );
    m_value.SetY ( GetDoubleFromXmlElement (element, "y") );
    if ( ! this->xyOnly() ) {
        m_value.SetZ ( GetDoubleFromXmlElement (element, "z") );
    }
    m_changing = true;        // Only call set once
    m_x.SetValue ( m_value.X() );
    m_y.SetValue ( m_value.Y() );
    m_z.SetValue ( m_value.Z() );
    m_changing = false;
    CallSetFunction();
}

void PropertyVertex::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    m_x.DefaultWriteToXmlElement ( prop_element );
    m_y.DefaultWriteToXmlElement ( prop_element );

    if ( ! this->xyOnly() ) {
        m_z.DefaultWriteToXmlElement ( prop_element );
    }
}

void PropertyVertex2d::operator = ( const Property& prop )
{
    const PropertyVertex2d * value = &(const PropertyVertex2d&)prop;
    SetValue(value->m_value);
}

Property * PropertyVertex2d::Clone ( ) const
{
    PropertyVertex2d * rtn = new PropertyVertex2d(m_name, m_title, NULL);
    rtn->m_value = m_value;
    rtn->m_x = m_x;
    rtn->m_y = m_y;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyTrsf::operator = ( const Property& prop )
{
    const PropertyTrsf * value = &(const PropertyTrsf&)prop;
    SetValue(value->m_value);
}

Property * PropertyTrsf::Clone ( ) const
{
    PropertyTrsf * rtn = new PropertyTrsf(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyTrsf::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    double a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34;
    a11 = GetDoubleFromXmlElement (element, "a11");
    a12 = GetDoubleFromXmlElement (element, "a12");
    a13 = GetDoubleFromXmlElement (element, "a13");
    a14 = GetDoubleFromXmlElement (element, "a14");
    a21 = GetDoubleFromXmlElement (element, "a21");
    a22 = GetDoubleFromXmlElement (element, "a22");
    a23 = GetDoubleFromXmlElement (element, "a23");
    a24 = GetDoubleFromXmlElement (element, "a24");
    a31 = GetDoubleFromXmlElement (element, "a31");
    a32 = GetDoubleFromXmlElement (element, "a32");
    a33 = GetDoubleFromXmlElement (element, "a33");
    a34 = GetDoubleFromXmlElement (element, "a34");

    m_value.SetValues(a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34);
    CallSetFunction();
}

void PropertyTrsf::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );
    AddDoubleToXmlElement ( prop_element, "a11", m_value.Value(1, 1) );
    AddDoubleToXmlElement ( prop_element, "a12", m_value.Value(1, 2) );
    AddDoubleToXmlElement ( prop_element, "a13", m_value.Value(1, 3) );
    AddDoubleToXmlElement ( prop_element, "a14", m_value.Value(1, 4) );
    AddDoubleToXmlElement ( prop_element, "a21", m_value.Value(2, 1) );
    AddDoubleToXmlElement ( prop_element, "a22", m_value.Value(2, 2) );
    AddDoubleToXmlElement ( prop_element, "a23", m_value.Value(2, 3) );
    AddDoubleToXmlElement ( prop_element, "a24", m_value.Value(2, 4) );
    AddDoubleToXmlElement ( prop_element, "a31", m_value.Value(3, 1) );
    AddDoubleToXmlElement ( prop_element, "a32", m_value.Value(3, 2) );
    AddDoubleToXmlElement ( prop_element, "a33", m_value.Value(3, 3) );
    AddDoubleToXmlElement ( prop_element, "a34", m_value.Value(3, 4) );
}


void PropertyAxis::operator = ( const Property& prop )
{
    const PropertyAxis * value = &(const PropertyAxis&)prop;
    SetValue(value->m_value);
}

Property * PropertyAxis::Clone ( ) const
{
    PropertyAxis * rtn = new PropertyAxis(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyAxis::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    gp_Pnt l ( GetDoubleFromXmlElement (element, "lx"),
               GetDoubleFromXmlElement (element, "ly"),
               GetDoubleFromXmlElement (element, "lz") );

    gp_Dir d ( GetDoubleFromXmlElement (element, "dx"),
               GetDoubleFromXmlElement (element, "dy"),
               GetDoubleFromXmlElement (element, "dz") );

    m_value.SetLocation(l);
    m_value.SetDirection(d);
    CallSetFunction();
}

void PropertyAxis::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );

    const gp_Pnt& l = m_value.Location();
    AddDoubleToXmlElement ( prop_element, "lx", l.X() );
    AddDoubleToXmlElement ( prop_element, "ly", l.Y() );
    AddDoubleToXmlElement ( prop_element, "lz", l.Z() );

    const gp_Dir& d = m_value.Direction();
    AddDoubleToXmlElement ( prop_element, "dx", d.X() );
    AddDoubleToXmlElement ( prop_element, "dy", d.Y() );
    AddDoubleToXmlElement ( prop_element, "dz", d.Z() );
}

void PropertyCoord::operator = ( const Property& prop )
{
    const PropertyCoord * value = &(const PropertyCoord&)prop;
    SetValue(value->m_value);
}

Property * PropertyCoord::Clone ( ) const
{
    PropertyCoord * rtn = new PropertyCoord(this->GetName(), m_title, NULL);
    rtn->m_value = m_value;
    rtn->SetOwner(this->GetOwner());
    return rtn;
}

void PropertyCoord::DefaultReadFromXmlElement ( TiXmlElement *element )
{
    if ( ! CallPreSetFunction() )
        return;

    gp_Pnt l ( GetDoubleFromXmlElement (element, "lx"),
               GetDoubleFromXmlElement (element, "ly"),
               GetDoubleFromXmlElement (element, "lz") );

    gp_Dir d ( GetDoubleFromXmlElement (element, "dx"),
               GetDoubleFromXmlElement (element, "dy"),
               GetDoubleFromXmlElement (element, "dz") );

    gp_Dir x ( GetDoubleFromXmlElement (element, "xx"),
               GetDoubleFromXmlElement (element, "xy"),
               GetDoubleFromXmlElement (element, "xz") );

    m_value.SetLocation(l);
    m_value.SetDirection(d);
    m_value.SetXDirection(x);
    CallSetFunction();
}

void PropertyCoord::DefaultWriteToXmlElement ( TiXmlElement *element ) const
{
    TiXmlElement* prop_element = new TiXmlElement( this->GetXmlName() );
    element->LinkEndChild ( prop_element );

    const gp_Pnt& l = m_value.Location();
    AddDoubleToXmlElement ( prop_element, "lx", l.X() );
    AddDoubleToXmlElement ( prop_element, "ly", l.Y() );
    AddDoubleToXmlElement ( prop_element, "lz", l.Z() );

    const gp_Dir& d = m_value.Direction();
    AddDoubleToXmlElement ( prop_element, "dx", d.X() );
    AddDoubleToXmlElement ( prop_element, "dy", d.Y() );
    AddDoubleToXmlElement ( prop_element, "dz", d.Z() );

    const gp_Dir& x = m_value.XDirection();
    AddDoubleToXmlElement ( prop_element, "xx", x.X() );
    AddDoubleToXmlElement ( prop_element, "xy", x.Y() );
    AddDoubleToXmlElement ( prop_element, "xz", x.Z() );
}
