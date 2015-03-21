#include "interface/Property.h"


class PropertyChange: public Undoable
{
public:
    Property * m_value;
    Property * m_prop;

    PropertyChange(Property * property);
    ~PropertyChange();

    void Run(bool redo);
    void RollBack();
    const wxChar* GetTitle(){return _("Property Change");}

private:

    void SwapPropertyValues();
};
