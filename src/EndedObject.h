// EndedObject.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"
#include "HPoint.h"

class EndedObject: public HeeksObj{
protected:
    PropertyColor m_color;

public:
	HPoint* A, *B;

	~EndedObject(void);
	EndedObject(const HeeksColor& color);
	EndedObject(const EndedObject& e);

	const EndedObject& operator=(const EndedObject &b);

	void SetColor(const HeeksColor &col) { m_color = col; }
	const HeeksColor& GetColor() const { return m_color; }

	// HeeksObj's virtual functions
	bool Stretch(const double *p, const double* shift, void* data);
	void ModifyByMatrix(const double* m);
	bool GetStartPoint(double* pos);
	bool GetEndPoint(double* pos);
	void CopyFrom(const HeeksObj* object){operator=(*((EndedObject*)object));}
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void glCommands(bool select, bool marked, bool no_color);
	HeeksObj* MakeACopyWithID();
	bool IsDifferent(HeeksObj* other);
//	void WriteBaseXML(TiXmlElement *element);

};
