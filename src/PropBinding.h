#pragma once

#include "wx/propgrid/propgrid.h"
#include "interface/MarkedObject.h"


class PropBindingTool : public Tool
{
public:
        MarkedObject *m_marked_object;

        PropBindingTool(MarkedObject *marked_object) : m_marked_object(marked_object){}

        // Tool's virtual functions
        const wxChar* GetTitle(){return _("Property Bindings");}

        void Run();
};
