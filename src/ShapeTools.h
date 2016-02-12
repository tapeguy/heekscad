// ShapeTools.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once


class CFaceList: public ObjList
{
public:
    CFaceList() : ObjList(FaceListType) { }
	HeeksObj *MakeACopy(void)const { return new CFaceList(*this); }
	const wxBitmap &GetIcon();
	bool DescendForUndo(){return false;}
	int GetMarkingFilter()const{return 0;}// not pickable
};

class CEdgeList: public ObjList
{
public:
    CEdgeList() : ObjList(EdgeListType) { }
	HeeksObj *MakeACopy(void)const { return new CEdgeList(*this); }
	const wxBitmap &GetIcon();
	bool DescendForUndo(){return false;}
	int GetMarkingFilter()const{return 0;}// not pickable
};

class CVertexList: public ObjList
{
public:
    CVertexList() : ObjList(VertexListType) { }
	HeeksObj *MakeACopy(void)const { return new CVertexList(*this); }
	const wxBitmap &GetIcon();
	bool DescendForUndo(){return false;}
	int GetMarkingFilter()const{return 0;}// not pickable
};

void CreateFacesAndEdges(TopoDS_Shape shape, CFaceList* faces, CEdgeList* edges, CVertexList* vertices);

wxString TopoDS_ToString(const TopoDS_Shape& shape, int level = 0);
