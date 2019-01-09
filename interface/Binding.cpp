
#include "stdafx.h"

#include "Binding.h"


Binding::Binding ( Property * p1, Property * p2 )
 : m_p1 ( p1 ), m_p2 ( p2 )
{
}

Binding::~Binding ()
{
}

void Binding::ReplaceProperty ( Property* orig, Property* new_prop )
{
    if ( m_p1 == orig )
    {
        m_p1 = new_prop;
    }
    else if ( m_p2 == orig )
    {
        m_p2 = new_prop;
    }
}

void Binding::RemoveProperty ( Property* prop )
{
    if ( m_p1 == prop )
    {
        m_p1 = NULL;
    }
    else if ( m_p2 == prop )
    {
        m_p2 = NULL;
    }
}

Property * Binding::GetOtherProperty ( const Property* prop ) const
{
    if ( prop == m_p1 )
    {
        return m_p2;
    }
    else if ( prop == m_p2 )
    {
        return m_p1;
    }
    return NULL;
}

EqualityBinding::EqualityBinding ( Property * subscriber, Property * observed )
: Binding ( subscriber, observed )
{
    // bidirectional binding
    subscriber->SetBinding ( this );
    subscriber->AddSubscriberBinding ( this );
    observed->SetBinding ( this );
    observed->AddSubscriberBinding ( this );
    this->Execute ( observed );
}

EqualityBinding::~EqualityBinding ( )
{
    if ( m_p1 )
    {
        m_p1->SetBindingDeleted ( );
        m_p1->RemoveSubscriberBinding ( this );
    }
    if ( m_p2 )
    {
        m_p2->SetBindingDeleted ( );
        m_p2->RemoveSubscriberBinding ( this );
    }
}

void EqualityBinding::Execute ( Property * observed )
{
    if ( observed == m_p1 )
    {
        *m_p2 = *m_p1;
    }
    else if ( observed == m_p2 )
    {
        *m_p1 = *m_p2;
    }
}
