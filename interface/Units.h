// Units.h

#include "EnumFactory.h"

#define ENUM_UNIT_TYPE(XX) \
	XX(UnitTypeMillimeter, = 0, _("mm")) \
	XX(UnitTypeInch,, _("inch")) \
	XX(MaximumUnitType,, _("invalid"))


DECLARE_ENUM(EnumUnitType, ENUM_UNIT_TYPE)

class Length
{
public:
    static double Conversion ( EnumUnitType from, EnumUnitType to );
};
