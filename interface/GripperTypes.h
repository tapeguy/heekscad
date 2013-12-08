// GripperTypes.h

#include "EnumFactory.h"

#define ENUM_GRIPPER_TYPE(XX) \
	XX(GripperTypeTranslate,) \
	XX(GripperTypeRotate,) \
	XX(GripperTypeRotateObject,) \
	XX(GripperTypeRotateObjectXY,) \
	XX(GripperTypeRotateObjectXZ,) \
	XX(GripperTypeRotateObjectYZ,) \
	XX(GripperTypeScale,) \
	XX(GripperTypeObjectScaleX,) \
	XX(GripperTypeObjectScaleY,) \
	XX(GripperTypeObjectScaleZ,) \
	XX(GripperTypeObjectScaleXY,) \
	XX(GripperTypeAngle,) \
	XX(GripperTypeStretch,) \


DECLARE_ENUM(EnumGripperType, ENUM_GRIPPER_TYPE);
