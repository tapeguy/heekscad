#include <stdafx.h>
#include "DomainObject.h"
#include "PropertyFactory.h"



DomainObject::DomainObject ( )
{
}


DomainObject::DomainObject ( const wxChar* name )
 : _name ( name )
{
}


DomainObject::DomainObject ( const DomainObject& copy )
{
    *this = copy;
}


const DomainObject& DomainObject::operator= ( const DomainObject &domobj )
{
    this->RemoveAllProperties ( );
    for ( DomainObjectIterator It = this->begin(); It != this->end(); It++ )
    {
        Property * new_prop = (*It)->Clone ( );
        this->AddProperty ( new_prop );
    }
    return *this;
}


DomainObject::DomainObject ( const wxChar* name,
                                   const PropertyFactory& factory,
                                   unsigned char * descriptors,
                                   int sizeof_descriptor )
: _name ( name )
{
    Property * property;
    for ( unsigned char * p = descriptors; *p != 0; p = p + sizeof_descriptor )
    {
        property = factory.CreateProperty ( p, this );
    }
}


DomainObject::~DomainObject ( )
{
//    _propertyList.DestroyValues ( );
}


void DomainObject::AddProperty ( Property * property )
{
    property->SetOwner ( this );
    _propertyList.push_back ( property );
}


void DomainObject::RemoveAllProperties ( )
{
    _propertyList.clear();
}


Property * DomainObject::GetProperty ( const wxChar * prop_name ) const
{
    for (DomainObjectIterator It = this->begin(); It != this->end(); It++ )
    {
        Property * property = *It;
        if ( property->GetName ( ) == prop_name )
            return property;
    }
    return NULL;
}


bool DomainObject::OnPropertySet ( Property& )
{
	return true;
}


void DomainObject::OnPropertyEdit ( Property& )
{
}


void DomainObject::GetProperties ( std::list<Property*> *list )
{
    list->insert(list->end(), _propertyList.begin(), _propertyList.end());
}


