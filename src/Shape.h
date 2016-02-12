// Shape.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/Material.h"
#include "ShapeData.h"
#include "ShapeTools.h"
#include "../interface/IdNamedObjList.h"

class CShape : public IdNamedObjList {
protected:
	int m_face_gl_list;
	int m_edge_gl_list;
	CBox m_box;
	wxLongLong m_creation_time;

	PropertyVertex m_centre;
	PropertyCheck m_calc_volume;
	PropertyDouble m_opacity;
	PropertyDouble m_volume;
	PropertyVertex m_centre_of_mass;

	void create_faces_and_edges();
	void delete_faces_and_edges();
	void CallMesh();
	virtual void MakeTransformedShape(const gp_Trsf &mat);
	virtual wxString StretchedName();

public:

	static const int ObjType = SolidType;


	static bool m_solids_found; // a flag for xml writing

    TopoDS_Shape m_shape;
	CFaceList* m_faces;
	CEdgeList* m_edges;
	CVertexList* m_vertices;
	CFace* m_picked_face;

	CShape();
	CShape(const TopoDS_Shape &shape, const wxChar* title, const HeeksColor& col, float opacity);
	CShape(const CShape& s);
	~CShape();

    void InitializeProperties();
    void GetProperties(std::list<Property *> *list);

	virtual const CShape& operator=(const CShape& s);

	// HeeksObj's virtual functions
	virtual bool UsesColor() { return true; }
	bool IsDifferent(HeeksObj* obj);
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	void KillGLLists(void);
	void ModifyByMatrix(const double* m);
	bool GetCentrePoint(double* pos);
	void GetTriangles(void(*callbackfunc)(const double* x, const double* n), double cusp, bool just_one_average_normal = true);
	double Area()const;
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void CopyFrom(const HeeksObj* object);
	void WriteXML(TiXmlNode *root);
	void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction);
	bool CanAddTo(HeeksObj* owner){return ((owner != NULL) && (owner->GetType() == GroupType));}
	bool Stretch(const double *p, const double* shift, void* data);
	bool DescendForUndo(){return false;}
	bool DrawAfterOthers(){return m_opacity < 0.9999;}

	const TopoDS_Shape& GetShape() const {return m_shape;}

	CFace* find(const TopoDS_Face &face);
	bool GetExtents(double* extents, const double* orig = NULL, const double* xdir = NULL, const double* ydir = NULL, const double* zdir = NULL);
	void CopyIDsFrom(const CShape* shape_from);
	float GetOpacity();
	void SetOpacity(float opacity);
	void CalculateVolumeAndCentre();

	static HeeksObj* CutShapes(std::list<HeeksObj*> &list, bool dodelete = true);
	static HeeksObj* FuseShapes(std::list<HeeksObj*> &list);
	static HeeksObj* CommonShapes(std::list<HeeksObj*> &list);
	static HeeksObj* SewFacesIntoShape(const std::list<TopoDS_Face> &faces);
	static void FilletOrChamferEdges(std::list<HeeksObj*> &list, double radius, bool chamfer_not_fillet = false);
	static bool ImportSolidsFile(const wxChar* filepath, bool undoably, std::map<int, CShapeData> *index_map = NULL, HeeksObj* paste_into = NULL);
	static bool ExportSolidsFile(const std::list<HeeksObj*>& objects, const wxChar* filepath, std::map<int, CShapeData> *index_map = NULL);
	static HeeksObj* MakeObject(const TopoDS_Shape &shape, const wxChar* title, SolidTypeEnum solid_type, const HeeksColor& col, float opacity);
	static bool IsTypeAShape(int t);
	static bool IsMatrixDifferentialScale(const double *mat);

	virtual void SetXMLElement(TiXmlElement* element){}
	virtual void SetFromXMLElement(TiXmlElement* pElem){}

	void Init();
};

