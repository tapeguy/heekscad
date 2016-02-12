#pragma once

#include "wx/propgrid/propgrid.h"


class BindButtonRenderer : public wxPGCellRenderer
{
private:

    wxButton* m_btn;
    wxPGCellRenderer* m_renderer;

public:

    BindButtonRenderer(wxButton* btn, wxPGCellRenderer* default_renderer)
     : m_btn(btn), m_renderer(default_renderer)
    {
    }

protected:

    virtual bool Render(wxDC &dc, const wxRect &rect,
                       const wxPropertyGrid *propertyGrid, wxPGProperty *property,
                       int column, int item, int flags) const;
};

class DoubleBindProperty : public wxFloatProperty {

private:

    wxButton* m_btn; // the button attached to the property
    mutable BindButtonRenderer m_renderer; // the button mover

public:

    DoubleBindProperty(wxWindow* parent, wxObjectEventFunction func, PropertyDouble* p);

protected:

    virtual wxPGCellRenderer* GetCellRenderer ( int column ) const {
        return ( column == 0 ) ? &m_renderer : wxFloatProperty::GetCellRenderer ( column );
    }
};

class LengthBindProperty : public wxFloatProperty {

private:

    wxButton* m_btn; // the button attached to the property
    mutable BindButtonRenderer m_renderer; // the button mover

public:

    LengthBindProperty(wxWindow* parent, wxObjectEventFunction func, PropertyLength* p);

protected:

    virtual wxPGCellRenderer* GetCellRenderer ( int column ) const {
        return ( column == 0 ) ? &m_renderer : wxFloatProperty::GetCellRenderer ( column );
    }
};
