// StlSolid.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#pragma once

#include "../interface/IdNamedObj.h"

class CStlTri
{
public:
	float x[3][3];
	CStlTri(){}
	CStlTri(const float* t);
	CStlTri(const double* t);
};

class CStlSolid : public IdNamedObj
{
private:
	HeeksColor m_color;
	int m_gl_list;
	CBox m_box;

	void read_from_file(const wxChar* filepath);

public:

	static const int ObjType = StlSolidType;


	std::list<CStlTri> m_list;

	CStlSolid();
	CStlSolid(const HeeksColor* col);
	CStlSolid(const wxChar* filepath, const HeeksColor* col);
#ifdef UNICODE
	CStlSolid(const std::wstring& filepath);
#endif
	CStlSolid( const CStlSolid & rhs );
	~CStlSolid();

	virtual const CStlSolid& operator=(const CStlSolid& s);

	int GetMarkingFilter()const{return StlSolidMarkingFilter;}
	int GetIDGroupType()const{return SolidType;}
	const wxBitmap &GetIcon();
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	void KillGLLists(void);
	void ModifyByMatrix(const double* m);
	void GetTriangles(void(*callbackfunc)(const double* x, const double* n), double cusp, bool just_one_average_normal = true);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void CopyFrom(const HeeksObj* object);
	HeeksObj *MakeACopy()const;
	void WriteXML(TiXmlNode *root);
	bool IsDifferent(HeeksObj* obj);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	void AddTriangle(float* t); // 9 floats
};

