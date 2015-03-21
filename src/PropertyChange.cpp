#include "stdafx.h"

#include "PropertyChange.h"


PropertyChange::PropertyChange(Property * property)
{
    m_value = property->Clone();
    if ( m_value->GetOwner() ) {
        m_value->GetOwner()->RemoveProperty(m_value);
    }
    m_prop = property;
}

PropertyChange::~PropertyChange()
{
    delete m_value;
}

void PropertyChange::Run(bool redo)
{
    if (redo) {
        SwapPropertyValues();
    }
    m_prop->CallEditFunction();
}

void PropertyChange::RollBack()
{
    SwapPropertyValues();
}

void PropertyChange::SwapPropertyValues()
{
    Property * temp = m_prop->Clone();
    if ( temp->GetOwner() ) {
        temp->GetOwner()->RemoveProperty(temp);
    }
    *m_prop = *m_value;
    *m_value = *temp;
    delete temp;
    m_prop->CallEditFunction();
}

