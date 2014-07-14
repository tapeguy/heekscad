// DynamicSolid.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/ObjList.h"

class DynamicSolid : public ObjList{
public:
	CFaceList* m_faces;
	CEdgeList* m_edges;
	CVertexList* m_vertices;
	std::list<TopoDS_Shape> m_shapes;

	DynamicSolid();
	~DynamicSolid();

	int GetType()const{return SolidType;}
	int GetMarkingFilter()const{return SolidMarkingFilter;}
	const wxChar* GetTypeString(void)const{return _("Solid");}
	void ReloadPointers();

	virtual SolidTypeEnum GetSolidType(){return SOLID_TYPE_UNKNOWN;}
	virtual gp_Trsf GetTransform(){return gp_Trsf();}

	void SetShapes(std::list<TopoDS_Shape>);
	void DrawShapes();
	virtual void Update(){}
};
