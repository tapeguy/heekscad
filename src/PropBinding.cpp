#include "stdafx.h"
#include "HeeksFrame.h"
#include "PropBinding.h"
#include "PropBindingDialog.h"


void PropBindingTool::Run()
{
    HeeksObj* subscriber = m_marked_object->GetObject ( );
    PropBindingDialog * dialog = new PropBindingDialog ( wxGetApp().m_frame );
    PropBindingSubscriberModel* subscriber_model = new PropBindingSubscriberModel ( subscriber );
    dialog->AddSubscriberModel ( subscriber_model );
    dialog->Show ( );
}

