#include "stdafx.h"

#include "PropertyFactory.h"

const char * PROP_READONLY = "READONLY";


Property * PropertyFactory::CreateProperty ( unsigned char * descriptor, DomainObject * owner ) const
{
    int i;
    Property * rtn = NULL;

    PropertyDescriptor * prop = (PropertyDescriptor *) descriptor;
    switch ( prop->type )
    {
    case PropertyStringType:
        rtn = new PropertyString(prop->name, prop->title, owner);
        break;

    case PropertyDoubleType:
        rtn = new PropertyDouble(prop->name, prop->title, owner);
        break;

    case PropertyLengthType:
        rtn = new PropertyLength(prop->name, prop->title, owner);
        break;

    case PropertyIntType:
        rtn = new PropertyInt(prop->name, prop->title, owner);
        break;

    case PropertyVertexType:
        rtn = new PropertyVertex(prop->name, prop->title, owner);
        break;

    case PropertyVertex2dType:
        rtn = new PropertyVertex2d(prop->name, prop->title, owner);
        break;

    case PropertyVectorType:
        rtn = new PropertyVector(prop->name, prop->title, owner);
        break;

    case PropertyChoiceType:
        rtn = new PropertyChoice(prop->name, prop->title, owner);
        break;

    case PropertyColorType:
        rtn = new PropertyColor(prop->name, prop->title, owner);
        break;

    case PropertyCheckType:
        rtn = new PropertyCheck(prop->name, prop->title, owner);
        break;

    case PropertyListType:
        rtn = new PropertyList(prop->name, prop->title, owner);
        break;

    case PropertyTrsfType:
        rtn = new PropertyTrsf(prop->name, prop->title, owner);
        break;

    case PropertyFileType:
        rtn = new PropertyFile(prop->name, prop->title, owner);
        break;

    case PropertyAxisType:
        rtn = new PropertyAxis(prop->name, prop->title, owner);
        break;

    case PropertyCoordType:
        rtn = new PropertyCoord(prop->name, prop->title, owner);
        break;

    default:
        return rtn;
    }

    for ( i = 0; i < sizeof(prop->attributes) / sizeof (PropertyAttribute); i++) {
        PropertyAttribute attribute = prop->attributes[i];
        if ( attribute.name == NULL )
            break;
        wxString name ( attribute.name );
        wxString value ( attribute.name );
        if (name == PROP_READONLY) {
            rtn->SetReadOnly(true);
        }
    }
    return rtn;
}
