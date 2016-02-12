#ifndef _domobj_h_
#define _domobj_h_

#include <list>
#include <iterator>


class Property;
class PropertyFactory;


class DomainObjectIterator : public std::iterator<std::input_iterator_tag, Property *>
{
private:

    std::list<Property*>::const_iterator It;

public:
    DomainObjectIterator() {}
    DomainObjectIterator(const std::list<Property*>::const_iterator& it) : It(it) {}
    DomainObjectIterator(const DomainObjectIterator& domit) : It(domit.It) {}
    DomainObjectIterator& operator++() {It.operator++(); return *this;}
    DomainObjectIterator operator++(int) {DomainObjectIterator tmp(*this); operator++(); return tmp;}
    bool operator==(const DomainObjectIterator& rhs) {return It==rhs.It;}
    bool operator!=(const DomainObjectIterator& rhs) {return It!=rhs.It;}
    Property * operator*() {return *It;}
};


class DomainObject
{
public:

    DomainObject ( );

    DomainObject ( const wxChar* name );

    DomainObject ( const wxChar* name,
                      const PropertyFactory& factory,
                      unsigned char * descriptors,
                      int sizeof_descriptor );

    DomainObject ( const DomainObject& copy );

    const DomainObject& operator= ( const DomainObject &domobj );

    virtual ~DomainObject ( );

    // Returns false if the property already exists - in which case it copies the value only
    virtual bool AddProperty ( Property * property );

    virtual void RemoveProperty ( Property * property );

    Property * GetProperty ( const wxChar * prop_name ) const;

    void RemoveAllProperties ( );

    const wxString& GetName ( ) const
    {
        return _name;
    }

    void SetName ( const wxString& name )
    {
        _name = name;
    }

    unsigned int PropertyCount ( ) const
    {
        return _propertyList.size();
    }

    DomainObjectIterator begin() const
    {
        return DomainObjectIterator ( _propertyList.begin() );
    }

    DomainObjectIterator end() const
    {
        return DomainObjectIterator ( _propertyList.end() );
    }

    // Override these callbacks:
    virtual bool OnPrePropertySet ( Property& );      // Before modification

    virtual void OnPropertySet ( Property& );

    virtual void GetProperties ( std::list<Property*> *list );

    virtual void OnPropertyEdit ( Property& );

protected:

    wxString _name;
    std::list<Property*> _propertyList;
};

#endif /* _domobj_h_ */
