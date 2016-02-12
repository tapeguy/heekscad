// Solid.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Shape.h"

class CSolid:public CShape{
protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);

public:

	CSolid(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);
	CSolid( const CSolid & rhs );

	~CSolid();

	virtual const CSolid& operator=(const CSolid& s){ CShape::operator =(s); return *this;}

	int GetType()const{return SolidType;}
	int GetMarkingFilter()const{return SolidMarkingFilter;}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;

	// CShape's virtual functions
	void SetXMLElement(TiXmlElement* element);
	void SetFromXMLElement(TiXmlElement* pElem);

	virtual SolidTypeEnum GetSolidType(){return SOLID_TYPE_UNKNOWN;}
};
