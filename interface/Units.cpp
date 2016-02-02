#include <wx/wx.h>
#include "Units.h"

DEFINE_ENUM(EnumUnitType, ENUM_UNIT_TYPE)


/* static */
double Length::Conversion ( EnumUnitType from, EnumUnitType to )
{
    switch ( from ) {
    case UnitTypeMillimeter :
        switch ( to ) {
        case UnitTypeMillimeter :
            return 1.0;
        case UnitTypeInch :
            return 0.0393701;
        }
        break;

    case UnitTypeInch :
        switch ( to ) {
        case UnitTypeMillimeter :
            return 25.4;
        case UnitTypeInch :
            return 1.0;
        }
        break;
    }
}
