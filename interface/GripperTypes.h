// GripperTypes.h

#include "EnumFactory.h"

#define ENUM_GRIPPER_TYPE(XX) \
	XX(GripperTypeTranslate,, _("translate")) \
	XX(GripperTypeRotate,, _("rotate")) \
	XX(GripperTypeRotateObject,, _("rotate object")) \
	XX(GripperTypeRotateObjectXY,, _("rotate object XY")) \
	XX(GripperTypeRotateObjectXZ,, _("rotate object XZ")) \
	XX(GripperTypeRotateObjectYZ,, _("rotate object YZ")) \
	XX(GripperTypeScale,, _("scale")) \
	XX(GripperTypeObjectScaleX,, _("scale X")) \
	XX(GripperTypeObjectScaleY,, _("scale Y")) \
	XX(GripperTypeObjectScaleZ,, _("scale Z")) \
	XX(GripperTypeObjectScaleXY,, _("scale XY")) \
	XX(GripperTypeAngle,, _("angle")) \
	XX(GripperTypeStretch,, _("stretch"))


DECLARE_ENUM(EnumGripperType, ENUM_GRIPPER_TYPE)
