// EndedObject.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"
#include "HPoint.h"

class EndedObject: public ObjList {

public:
	HPoint* A, *B;

	EndedObject(int obj_type);
	EndedObject(int obj_type, const HeeksColor& color);
	EndedObject(const EndedObject& e);
    ~EndedObject();

	const EndedObject& operator=(const EndedObject &b);

	// HeeksObj's virtual functions
	virtual bool UsesColor() { return true; }
	bool Stretch(const double *p, const double* shift, void* data);
	void ModifyByMatrix(const double* m);
	bool GetStartPoint(double* pos);
	bool GetEndPoint(double* pos);
	void CopyFrom(const HeeksObj* object){operator=(*((EndedObject*)object));}
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void glCommands(bool select, bool marked, bool no_color);
	bool IsDifferent(HeeksObj* other);
};
