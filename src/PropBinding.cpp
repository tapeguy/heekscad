#include "stdafx.h"
#include "PropBinding.h"


static const wxBitmap GetIcon()
{
    static wxBitmap* icon = NULL;
    if(icon == NULL)
        icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/bitmaps/binding.png")));
    return *icon;
}


bool BindButtonRenderer::Render(wxDC &dc, const wxRect &rect,
                       const wxPropertyGrid *pg, wxPGProperty *property,
                       int column, int item, int flags) const {

    m_renderer->Render(dc, rect, pg, property, column, item, flags);
    wxRect box = rect;
    box.SetLeft ( rect.GetRight ( ) - GetIcon().GetWidth() - 12 );
    box.SetWidth ( GetIcon().GetWidth() + 14 );
    box.SetTop ( rect.GetTop ( ) );
    m_btn->SetSize ( box );
    m_btn->Show ( );

    return true;
}

DoubleBindProperty::DoubleBindProperty(wxWindow* parent,
                                       wxObjectEventFunction func,
                                       PropertyDouble* p)
 : wxFloatProperty(p->GetShortString(), wxPG_LABEL, *p),
   m_btn(new wxBitmapButton(parent, wxID_ANY, GetIcon())),
   m_renderer(m_btn, wxFloatProperty::GetCellRenderer (0))
{
    // connect the handler to the button
    m_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, func);
    m_btn->Hide(); // when it's off the grid, it's not rendered
                  // (thus not moved properly)
    m_btn->SetClientData(p);
}

LengthBindProperty::LengthBindProperty(wxWindow* parent,
                                       wxObjectEventFunction func,
                                       PropertyLength* p)
 : wxFloatProperty(p->GetShortString(), wxPG_LABEL, p->GetActualValue()),
   m_btn(new wxBitmapButton(parent, wxID_ANY, GetIcon())),
   m_renderer(m_btn, wxFloatProperty::GetCellRenderer (0))
{
    SetAttribute(wxPG_ATTR_UNITS, GetShortString(p->GetUnits()));

    // connect the handler to the button
    m_btn->SetClientData(p);
    m_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, func);
    m_btn->Hide(); // when it's off the grid, it's not rendered
                  // (thus not moved properly)
}
