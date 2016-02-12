// ObjPropsCanvas.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "PropBinding.h"
#include "PropBindingDialog.h"
#include "ObjPropsCanvas.h"
#include "../interface/Property.h"
#include "../interface/ToolImage.h"
#include "wx/propgrid/propgrid.h"
#include "HeeksFrame.h"
#include "MarkedList.h"
#include "../interface/MarkedObject.h"
#include <wx/choicdlg.h>

BEGIN_EVENT_TABLE(CObjPropsCanvas, wxScrolledWindow)
	EVT_SIZE(CObjPropsCanvas::OnSize)

	// This occurs when a property value changes
	EVT_PG_CHANGED( -1, CObjPropsCanvas::OnPropertyGridChange )

	// unit editing
    EVT_PG_LABEL_EDIT_BEGIN( -1, CObjPropsCanvas::OnPropertyEditBegin )
//    EVT_PG_LABEL_EDIT_ENDING( -1,  CObjPropsCanvas::OnPropertyEditEnding )
END_EVENT_TABLE()


CObjPropsCanvas::CObjPropsCanvas(wxWindow* parent)
        : CPropertiesCanvas(parent)
{
    m_pg->SetColumnCount(3);
    m_pg->SetColumnProportion(0, 3);
    m_pg->SetColumnProportion(1, 2);
    m_pg->SetColumnProportion(2, 1);
    m_pg->MakeColumnEditable(2);
	m_toolBar = NULL;
    AddToolBar();
}

void CObjPropsCanvas::AddToolBar()
{
	if(m_toolBar)
	    delete m_toolBar;

	// make a toolbar for the current input modes's tools
	m_toolBar = new wxAuiToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	m_toolBar->SetToolBitmapSize(wxSize(ToolImage::GetBitmapSize(), ToolImage::GetBitmapSize()));
	m_toolBar->Realize();
}

CObjPropsCanvas::~CObjPropsCanvas()
{
}

void CObjPropsCanvas::OnSize(wxSizeEvent& event)
{
    CPropertiesCanvas::OnSize(event);
	Resize();
	event.Skip();
}

void CObjPropsCanvas::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	CPropertiesCanvas::OnPropertyGridChange(event);
}

void CObjPropsCanvas::OnPropertyEditBegin( wxPropertyGridEvent& event )
{
    wxPGProperty* selected = event.GetProperty();
    unsigned int colIndex = event.GetColumn();
    Property * property = (Property *)GetProperty(selected);
    if(property != NULL &&
       colIndex == 2 &&
       (property->GetPropertyType() == PropertyLengthType ||
        property->GetPropertyType() == PropertyVertexType ||
        property->GetPropertyType() == PropertyVertex2dType))
    {
        PropertyLength * length = (PropertyLength *)property;
        wxArrayString choices;
        EnumUnitType i;
        for(i = (EnumUnitType)0; i < MaximumUnitType; i++)
        {
            choices.Add(GetShortString(i));
        }
        wxSingleChoiceDialog unit_dialog ( m_pg->GetPanel(), _("Select a unit"), _("Unit"), choices);
        if (unit_dialog.ShowModal() == wxID_OK)
        {
            int choice = unit_dialog.GetSelection();
            length->SetUnits((EnumUnitType)choice);
            RefreshByRemovingAndAddingAll();
        }
    }
    event.Veto(true);
}

void CObjPropsCanvas::OnBindButton(wxCommandEvent& event)
{
    wxButton* btn = (wxButton*) event.GetEventObject ( );
    Property* p = (Property*) btn->GetClientData ( );
    HeeksObj* subscriber = (HeeksObj*) p->GetOwner ( );

    wxGetApp().PickObjects(_("Pick object to bind to"), MarkedList::all_filters, true);
    if(wxGetApp().m_marked_list->size() == 0)return;

    HeeksObj * observed = wxGetApp ( ).m_marked_list->list ( ).front ( );
    wxGetApp ( ).m_marked_list->Clear ( true );

    PropBindingDialog * dialog = new PropBindingDialog ( wxGetApp().m_frame );
    PropBindingSubscriberModel* subscriber_model = new PropBindingSubscriberModel ( subscriber );
    dialog->AddSubscriberModel ( subscriber_model );
    PropBindingObservedModel* observed_model = new PropBindingObservedModel ( observed );
    dialog->AddObservedModel ( observed_model );
    dialog->Show();
}

void CObjPropsCanvas::AddProperty(Property* p, wxPGProperty* parent_prop)
{

    if (!p->IsVisible() || p->GetName().IsEmpty()) {
        return;
    }

    if(p->IsReadOnly()) {
        CPropertiesCanvas::AddProperty(p, parent_prop);
        return;
    }

    switch(p->GetPropertyType()) {
    case PropertyStringType:
    case PropertyColorType:
    case PropertyChoiceType:
    case PropertyCheckType:
    case PropertyListType:
    case PropertyFileType:
        CPropertiesCanvas::AddProperty(p, parent_prop);
        break;
    case PropertyDoubleType:
        {
            PropertyDouble* d = (PropertyDouble*)p;
            wxPGProperty *new_prop = new DoubleBindProperty(
                    m_pg, wxCommandEventHandler(CObjPropsCanvas::OnBindButton), d);

            Append(parent_prop, new_prop, p);
            if(p->IsHighlighted()) {
                m_pg->SetPropertyBackgroundColour(new_prop, wxColour(71, 141, 248), false);
            }
        }
        break;
    case PropertyLengthType:
        {
            PropertyLength * l = (PropertyLength*)p;
            wxPGProperty *new_prop = new LengthBindProperty(
                    m_pg, wxCommandEventHandler(CObjPropsCanvas::OnBindButton), l);

            Append(parent_prop, new_prop, p);
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

    case PropertyVertex2dType:
    case PropertyVertexType:
        {
            wxPGProperty* new_prop = new wxPropertyCategory(p->GetShortString(),wxPG_LABEL);
            PropertyVertex* vertex = (PropertyVertex*)p;
            Append(parent_prop, new_prop, p);
            unsigned int number_of_axes = vertex->xyOnly() ? 2 : 3;

            wxPGProperty* x_prop = new wxFloatProperty(_("x"), p->GetShortString() + _(".x"), vertex->XProp().GetActualValue());
            x_prop->SetAttribute(wxPG_ATTR_UNITS, GetShortString(vertex->XProp().GetUnits()));
            if(p->IsReadOnly()) {
                x_prop->ChangeFlag(wxPG_PROP_READONLY, true);
            }
            Append( new_prop, x_prop, p );
            wxPGProperty* y_prop = new wxFloatProperty(_("y"), p->GetShortString() + _(".y"), vertex->YProp().GetActualValue());
            y_prop->SetAttribute(wxPG_ATTR_UNITS, GetShortString(vertex->YProp().GetUnits()));
            if(p->IsReadOnly()) {
                y_prop->ChangeFlag(wxPG_PROP_READONLY, true);
            }
            Append( new_prop, y_prop, p );
            if(number_of_axes == 3)
            {
                wxPGProperty* z_prop = new wxFloatProperty(_("z"), p->GetShortString() + _(".z"), vertex->ZProp().GetActualValue());
                z_prop->SetAttribute(wxPG_ATTR_UNITS, GetShortString(vertex->ZProp().GetUnits()));
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
    }
}

void CObjPropsCanvas::RefreshByMerging()
{
	wxGetApp().m_frame->ClearToolBar(m_toolBar);

	HeeksObj* marked_object = NULL;
	if(wxGetApp().m_marked_list->size() == 1)
	{
		marked_object = (*wxGetApp().m_marked_list->list().begin());
	}

	if(wxGetApp().m_marked_list->size() > 0)
	{
		// get the desired property list
		std::list<Property *> list;
		wxGetApp().m_marked_list->GetProperties(&list);

		CPropertiesCanvas::RefreshByMerging(list);

		// add toolbar buttons
		std::list<Tool*> t_list;
		MarkedObjectOneOfEach mo(0, marked_object, 1, 0, NULL);
		wxGetApp().m_marked_list->GetTools(&mo, t_list, NULL, false);
		for(std::list<Tool*>::iterator It = t_list.begin(); It != t_list.end(); It++)
		{
			Tool* tool = *It;
			if(tool)wxGetApp().m_frame->AddToolBarTool(m_toolBar, tool);
		}

		m_toolBar->Realize();
	}

	Resize();
}

void CObjPropsCanvas::RefreshByRemovingAndAddingAll()
{
	ClearProperties();
	wxGetApp().m_frame->ClearToolBar(m_toolBar);

	HeeksObj* marked_object = NULL;
	if(wxGetApp().m_marked_list->size() == 1)
	{
		marked_object = (*wxGetApp().m_marked_list->list().begin());
	}

	if(wxGetApp().m_marked_list->size() > 0)
	{
		// use the property list too
		std::list<Property *> list;
		wxGetApp().m_marked_list->GetProperties(&list);
		for(std::list<Property*>::iterator It = list.begin(); It != list.end(); It++)
		{
		    Property* property = *It;
			AddProperty((Property *)property);
		}

		// add toolbar buttons
		std::list<Tool*> t_list;
		MarkedObjectOneOfEach mo(0, marked_object, 1, 0, NULL);
		wxGetApp().m_marked_list->GetTools(&mo, t_list, NULL, false);
		for(std::list<Tool*>::iterator It = t_list.begin(); It != t_list.end(); It++)
		{
			Tool* tool = *It;
			if(tool)wxGetApp().m_frame->AddToolBarTool(m_toolBar, tool);
		}

		m_toolBar->Realize();
	}
	Resize();
}

void CObjPropsCanvas::Resize()
{
	// resize property grid and toolbar

	// change size for toolbar
	wxSize size = GetClientSize();
	wxSize pg_size = size;

	if(m_toolBar->GetToolCount() > 0){
		int toolbar_height = ToolImage::GetBitmapSize() + EXTRA_TOOLBAR_HEIGHT;
		pg_size = wxSize(size.x, size.y - toolbar_height);
		m_toolBar->SetSize(0, pg_size.y , size.x, toolbar_height );
		m_toolBar->Show();
	}
	else
	{
		m_toolBar->Show(false);
	}

	// change size for property grid
	m_pg->SetSize(0, 0, pg_size.x, pg_size.y);
}

void CObjPropsCanvas::WhenMarkedListChanges(bool selection_cleared, const std::list<HeeksObj *>* added_list, const std::list<HeeksObj *>* removed_list)
{
	RefreshByRemovingAndAddingAll();
}

void CObjPropsCanvas::OnChanged(const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed, const std::list<HeeksObj*>* modified)
{
	RefreshByMerging();
}

void CObjPropsCanvas::RefreshProperties2()
{
	RefreshByMerging();
}
