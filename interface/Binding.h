
#if !defined Binding_HEADER
#define Binding_HEADER

typedef enum
{
    BindingInvalidType = 0,
    BindingEqualityType = 1
} BindingType;


// Abstract base class for all Bindings
//
class Binding {

protected:

    Property * m_p1;
    Property * m_p2;

public:

    Binding ( Property* p1, Property* p2 );

    virtual ~Binding ( );

    virtual void ReplaceProperty ( Property* orig, Property* new_prop );

    virtual void RemoveProperty ( Property* prop );

    virtual void Execute ( Property * observed ) = 0;

    virtual Property * GetOtherProperty ( Property * prop );

    virtual int GetBindingType() const {return BindingInvalidType;}
    virtual const wxChar* GetPropertyTypeName() const = 0;
};


// This is a bidirectional binding.  Modifying either property will propagate to the other.
//
class EqualityBinding : public Binding {

protected:

    void Execute ( Property * observed );

public:

    EqualityBinding ( Property * subscriber, Property * observed );

    ~EqualityBinding ( );

    int GetBindingType() const {return BindingEqualityType;}
    const wxChar* GetPropertyTypeName() const {return _("EQUALS");}
};

#endif
