// PropertiesCanvas.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "PropertiesCanvas.h"
#include "../interface/MarkedObject.h"
#include "../interface/Property.h"
#include "../interface/PropertyList.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include "MarkedList.h"
#include "HeeksFrame.h"
#include "CoordinateSystem.h"

BEGIN_EVENT_TABLE(CPropertiesCanvas, wxScrolledWindow)
	EVT_SIZE(CPropertiesCanvas::OnSize)

        // This occurs when a property value changes
        EVT_PG_CHANGED( -1, CPropertiesCanvas::OnPropertyGridChange )
END_EVENT_TABLE()

CPropertiesCanvas::CPropertiesCanvas(wxWindow* parent)
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				   wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE),m_frozen(false), m_refresh_wanted_on_thaw(false)
{
	// Assumes code is in frame/dialog constructor

	// Construct wxPropertyGrid control
	m_pg = new wxPropertyGrid(
		this, // parent
		-1, // id
		wxDefaultPosition, // position
		wxDefaultSize, // size
		// Some specific window styles - for all additional styles,
		// see Modules->PropertyGrid Window Styles
		wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
		// Default style
		wxPG_DEFAULT_STYLE | wxBORDER_THEME );

	m_pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );

	wxGetApp().RegisterObserver(this);
}

CPropertiesCanvas::~CPropertiesCanvas()
{
	wxGetApp().RemoveObserver(this);
	delete m_pg;
}

void CPropertiesCanvas::OnSize(wxSizeEvent& event)
{
	wxScrolledWindow::HandleOnSize(event);

	wxSize size = GetClientSize();
	m_pg->SetSize(0, 0, size.x, size.y );

    event.Skip();
}

void CPropertiesCanvas::ClearProperties()
{
	m_pg->Clear();
	pmap.clear();
}

void CPropertiesCanvas::RemoveProperty(Property* property)
{
	for(std::map<wxPGProperty*, Property*>::iterator It = pmap.begin(); It != pmap.end(); It++)
	{
	    Property* cursor = It->second;
		if(property == cursor)
		{
			m_pg->DeleteProperty((wxPGProperty*)It->first);
			pmap.erase(It);
			return;
		}
	}
}

void CPropertiesCanvas::Append(wxPGProperty* parent_prop, wxPGProperty* new_prop, Property* property)
{
	if(parent_prop) {
		m_pg->AppendIn(parent_prop, new_prop);
	}
	else {
		m_pg->AppendIn(m_pg->GetRoot(), new_prop);
	}

	pmap.insert(std::pair<wxPGProperty*, Property*>( new_prop, property));
}

void CPropertiesCanvas::AddProperty(Property* p, wxPGProperty* parent_prop)
{
	if (!p->IsVisible()) {
		return;
	}

	switch(p->GetPropertyType()){
	case PropertyStringType:
		{
			wxPGProperty *new_prop = new wxStringProperty(p->GetShortString(),wxPG_LABEL, *(PropertyString*)p);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p);
            if(p->IsHighlighted()) {
                m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
            }
		}
		break;
	case PropertyDoubleType:
	case PropertyLengthType:
		{
			wxPGProperty *new_prop = new wxFloatProperty(p->GetShortString(),wxPG_LABEL, *(PropertyDouble*)p);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p);
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	case PropertyIntType:
		{
			wxPGProperty *new_prop = new wxIntProperty(p->GetShortString(),wxPG_LABEL, *(PropertyInt*)p);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p);
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	case PropertyColorType:
		{
			const HeeksColor& col = *(PropertyColor*)p;
			wxColour wcol(col.red, col.green, col.blue);
			wxPGProperty *new_prop = new wxColourProperty(p->GetShortString(),wxPG_LABEL, wcol);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p);
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	case PropertyChoiceType:
		{
			wxArrayString array_string;
			wxArrayInt array_ids;
			int counter = 0;
			std::list< wxString >::iterator It;
			for(It = ((PropertyChoice*)p)->m_choices.begin(); It != ((PropertyChoice*)p)->m_choices.end(); It++){
				array_string.Add(wxString(It->c_str()));
				array_ids.Add(counter);
				counter++;
			}
			wxPGProperty *new_prop = new wxEnumProperty(p->GetShortString(),wxPG_LABEL, array_string, array_ids, *(PropertyChoice*)p);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p );
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	case PropertyVertexType:
		{
			wxPGProperty* new_prop = new wxPropertyCategory(p->GetShortString(),wxPG_LABEL);
			PropertyVertex* vertex = (PropertyVertex*)p;
			Append( parent_prop, new_prop, p );
			unsigned int number_of_axes = vertex->xyOnly() ? 2 : 3;

			wxPGProperty* x_prop = new wxFloatProperty(_("x"), p->GetShortString() + _(".x"), vertex->X());
			if(p->IsReadOnly()) {
				x_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( new_prop, x_prop, p );

			wxPGProperty* y_prop = new wxFloatProperty(_("y"), p->GetShortString() + _(".y"), vertex->Y());
			if(p->IsReadOnly()) {
				y_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( new_prop, y_prop, p );

			if(number_of_axes == 3)
			{
				wxPGProperty* z_prop = new wxFloatProperty(_("z"), p->GetShortString() + _(".z"), vertex->Z());
				if(p->IsReadOnly()) {
					z_prop->ChangeFlag(wxPG_PROP_READONLY, true);
				}
				Append( new_prop, z_prop, p );
			}
		}
		break;
	case PropertyTrsfType:
		{
			const gp_Trsf& trsf = *(PropertyTrsf*)p;
			gp_XYZ xyz = trsf.TranslationPart();
			gp_Dir xaxis(1, 0, 0);
			xaxis.Transform(trsf);
			gp_Dir yaxis(0, 1, 0);
			yaxis.Transform(trsf);

			double vertical_angle = 0;
			double horizontal_angle = 0;
			double twist_angle = 0;
			CoordinateSystem::AxesToAngles(xaxis, yaxis, vertical_angle, horizontal_angle, twist_angle);

			wxPGProperty* new_prop = new wxPropertyCategory(p->GetShortString(),wxPG_LABEL);
			Append( parent_prop, new_prop, p );
			wxPGProperty* x_prop = new wxFloatProperty(_("x"),wxPG_LABEL,xyz.X());
			if(p->IsReadOnly())x_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, x_prop, p );
			wxPGProperty* y_prop = new wxFloatProperty(_("y"),wxPG_LABEL,xyz.Y());
			if(p->IsReadOnly())y_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, y_prop, p );
			wxPGProperty* z_prop = new wxFloatProperty(_("z"),wxPG_LABEL,xyz.Z());
			if(p->IsReadOnly())z_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, z_prop, p );
			wxPGProperty* v_prop = new wxFloatProperty(_("vertical angle"),wxPG_LABEL,vertical_angle);
			if(p->IsReadOnly())v_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, v_prop, p );
			wxPGProperty* h_prop = new wxFloatProperty(_("horizontal angle"),wxPG_LABEL,horizontal_angle);
			if(p->IsReadOnly())h_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, h_prop, p );
			wxPGProperty* t_prop = new wxFloatProperty(_("twist angle"),wxPG_LABEL,twist_angle);
			if(p->IsReadOnly())t_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			Append( new_prop, t_prop, p );
		}
		break;
	case PropertyCheckType:
		{
			wxPGProperty* new_prop = new wxBoolProperty(p->GetShortString(),wxPG_LABEL, ((PropertyCheck*)p)->IsSet());
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p );
			m_pg->SetPropertyAttribute(new_prop, wxPG_BOOL_USE_CHECKBOX, true);
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	case PropertyListType:
		{
			wxPGProperty* new_prop = new wxPropertyCategory(p->GetShortString(),wxPG_LABEL);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p );
			std::list< Property* > proplist = *(PropertyList *)p;
			std::list< Property* >::iterator It;
			for(It = proplist.begin(); It != proplist.end(); It++){
				Property* p2 = *It;
				AddProperty(p2, new_prop);
			}
		}
		break;
	case PropertyFileType:
		{
			wxPGProperty *new_prop = new wxFileProperty(p->GetShortString(),wxPG_LABEL, *(PropertyFile*)p);
			if(p->IsReadOnly()) {
				new_prop->ChangeFlag(wxPG_PROP_READONLY, true);
			}
			Append( parent_prop, new_prop, p);
			if(p->IsHighlighted()) {
				m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
			}
		}
		break;
	}
}

Property* CPropertiesCanvas::GetProperty(wxPGProperty* p)
{
	std::map<wxPGProperty*, Property*>::iterator FindIt;
	FindIt = pmap.find(p);
	if(FindIt == pmap.end())return NULL;
	return FindIt->second;
}

void CPropertiesCanvas::OnPropertyGridChange( wxPropertyGridEvent& event ) {
	wxPGProperty* p = event.GetProperty();

	Property * property = (Property *)GetProperty(p);
	if(property == NULL)
	    return;

	wxGetApp().CreateUndoPoint();
	switch(property->GetPropertyType()) {
	case PropertyStringType:
		((PropertyString*)property)->SetValue ( event.GetPropertyValue().GetString() );
		break;
	case PropertyDoubleType:
		((PropertyDouble*)property)->SetValue ( event.GetPropertyValue().GetDouble() );
		break;
	case PropertyLengthType:
		((PropertyLength*)property)->SetValue ( event.GetPropertyValue().GetDouble() );
		break;
	case PropertyIntType:
		((PropertyInt*)property)->SetValue ( event.GetPropertyValue().GetLong() );
		break;
	case PropertyColorType:
		{
			wxAny var = event.GetPropertyValue();
			wxColour wcol = wxANY_AS(var, wxColour);
			HeeksColor col(wcol.Red(), wcol.Green(), wcol.Blue());
			((PropertyColor*)property)->SetValue ( col );
		}
		break;
	case PropertyVertexType:
		{
			if(p->GetLabel()[0] == 'x'){
				((PropertyVertex*)property)->SetX( event.GetPropertyValue().GetLong() );
			}
			else if(p->GetLabel()[0] == 'y'){
				((PropertyVertex*)property)->SetY( event.GetPropertyValue().GetLong() );
			}
			else if(p->GetLabel()[0] == 'z'){
				((PropertyVertex*)property)->SetZ( event.GetPropertyValue().GetLong() );
			}
		}
		break;
	case PropertyTrsfType:
		{
			const gp_Trsf& trsf = *(PropertyTrsf*)p;
			gp_Pnt pnt = trsf.TranslationPart();

			gp_Dir xaxis(1, 0, 0);
			xaxis.Transform(trsf);
			gp_Dir yaxis(0, 1, 0);
			yaxis.Transform(trsf);

			double vertical_angle = 0;
			double horizontal_angle = 0;
			double twist_angle = 0;
			CoordinateSystem::AxesToAngles(xaxis, yaxis, vertical_angle, horizontal_angle, twist_angle);

			if(p->GetName()[0] == 'x'){
				pnt.SetX( event.GetPropertyValue().GetDouble() );
			}
			else if(p->GetName()[0] == 'y'){
				pnt.SetY( event.GetPropertyValue().GetDouble() );
			}
			else if(p->GetName()[0] == 'z'){
				pnt.SetZ( event.GetPropertyValue().GetDouble() );
			}
			else if(p->GetName()[0] == 'v'){
				vertical_angle = event.GetPropertyValue().GetDouble();
			}
			else if(p->GetName()[0] == 'h'){
				horizontal_angle = event.GetPropertyValue().GetDouble();
			}
			else if(p->GetName()[0] == 't'){
				twist_angle = event.GetPropertyValue().GetDouble();
			}

			CoordinateSystem::AnglesToAxes(vertical_angle, horizontal_angle, twist_angle, xaxis, yaxis);
			((PropertyTrsf*)property)->SetValue ( make_matrix(pnt, xaxis, yaxis) );
		}
		break;
	case PropertyChoiceType:
		((PropertyChoice*)property)->SetValue ( event.GetPropertyValue().GetLong() );
		break;
	case PropertyCheckType:
		((PropertyChoice*)property)->SetValue ( event.GetPropertyValue().GetBool() );
		break;
	case PropertyListType:
		{
		}
		break;
	case PropertyFileType:
		((PropertyFile*)property)->SetValue ( event.GetPropertyValue().GetString() );
		break;
	}
    property->CallEditFunction();
	wxGetApp().Changed();
	wxGetApp().Repaint();
}

void CPropertiesCanvas::DeselectProperties()
{
	m_pg->DoSelectProperty(NULL);
}

void CPropertiesCanvas::RefreshProperties()
{
	if(m_frozen)
	{
		m_refresh_wanted_on_thaw = true;
	}
	else
	{
		RefreshProperties2();
	}
}

void CPropertiesCanvas::Freeze()
{
	m_frozen = true;
}

void CPropertiesCanvas::Thaw()
{
	m_frozen = false;
	if(m_refresh_wanted_on_thaw)
	{
		RefreshProperties2();
		m_refresh_wanted_on_thaw = false;
	}
}
