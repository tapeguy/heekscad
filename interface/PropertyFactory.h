#if !defined PropertyFactory_HEADER
#define PropertyFactory_HEADER

#include "Property.h"

extern const char * PROP_READONLY;


struct PropertyAttribute
{
    const char* name;
    const char* value;
};

struct PropertyDescriptor
{
    PropertyType type;
    const wxChar* name;
    const wxChar* title;
    PropertyAttribute attributes[10];
};


class PropertyFactory
{
public:

    Property * CreateProperty ( unsigned char * descriptor, DomainObject * owner = NULL ) const;
};

#endif /* PropertyFactory_HEADER */
