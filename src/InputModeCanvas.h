// InputModeCanvas.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "PropertiesCanvas.h"

class CInputModeCanvas: public CPropertiesCanvas, public DomainObject
{
private:
	wxAuiToolBar *m_toolBar;
	std::list<Tool*> m_previous_tools; // to decide whether to remove and reshow toolbar
	PropertyString m_input_mode;

	void RefreshByRemovingAndAddingAll();

public:
    CInputModeCanvas(wxWindow* parent);
    virtual ~CInputModeCanvas();

    //virtual void OnDraw(wxDC& dc);
    void OnSize(wxSizeEvent& event);

public:
	// CPropertiesCanvas's virtual functions
	void RefreshProperties2();

	void AddToolBar();

    DECLARE_NO_COPY_CLASS(CInputModeCanvas)
    DECLARE_EVENT_TABLE()
};
