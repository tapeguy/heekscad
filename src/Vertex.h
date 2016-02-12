// Vertex.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"

class CFace;
class CEdge;

class HVertex : public HeeksObj
{
private:
	TopoDS_Vertex m_topods_vertex;
	std::list<CEdge*>::iterator m_edgeIt;

	void FindEdges();

public:

	static const int ObjType = VertexType;


	std::list<CEdge*> m_edges;
	PropertyVertex m_p;

	HVertex(const TopoDS_Vertex &vertex);
	~HVertex();

    // HeeksObj's virtual functions
    void InitializeProperties();
	int GetMarkingFilter()const{return VertexMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	HeeksObj *MakeACopy(void)const{ return new HVertex(*this);}
	const wxBitmap &GetIcon();

	TopoDS_Vertex& Vertex(){return m_topods_vertex;}
	void ModifyByMatrix(const double *m);
	CEdge* GetFirstEdge();
	CEdge* GetNextEdge();
	CShape* GetParentBody();
};

