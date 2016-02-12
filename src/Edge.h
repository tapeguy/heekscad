// Edge.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"
#include "../interface/Property.h"

class CFace;
class CShape;
class HVertex;

class CEdge : public HeeksObj
{
private:
	CBox m_box;
	TopoDS_Edge m_topods_edge;
	double m_start_x;
	double m_start_y;
	double m_start_z;
	double m_end_x;
	double m_end_y;
	double m_end_z;
	double m_start_u;
	double m_end_u;
	double m_start_tangent_x;
	double m_start_tangent_y;
	double m_start_tangent_z;
	int m_isClosed;
	int m_isPeriodic;
	bool m_orientation;
	HVertex* m_vertex0;
	HVertex* m_vertex1;
	PropertyLength m_length;
	PropertyString m_curve;

	void FindVertices();

public:

	static const int ObjType = EdgeType;


	std::list<CFace*>::iterator m_faceIt;
	std::list<CFace*> m_faces;
	std::list<bool> m_face_senses;
	double m_midpoint[3];
	bool m_midpoint_calculated;

	CEdge(const TopoDS_Edge &edge);
	~CEdge();
	int m_temp_attr; // not saved with the model

	void InitializeProperties();
	void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return EdgeMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	HeeksObj *MakeACopy(void)const{ return new CEdge(*this); }
	const wxBitmap &GetIcon();
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void WriteXML(TiXmlNode *root);
	bool GetMidPoint(double* pos);
	bool GetStartPoint(double* pos);
	bool GetEndPoint(double* pos);

	const TopoDS_Edge &Edge(){return m_topods_edge;}
	//void Blend(double radius);
 	void Blend(double radius,bool chamfer_not_fillet);
	CFace* GetFirstFace();
	CFace* GetNextFace();
	int GetCurveType();
	wxString GetCurveTypeStr();
	void GetCurveParams(double* start, double* end, double* uStart, double* uEnd, int* Reversed);
	void GetCurveParams2(double *uStart, double *uEnd, int *isClosed, int *isPeriodic);
	bool InFaceSense(CFace* face);
	void Evaluate(double u, double *p, double *tangent);
	bool GetClosestPoint(const gp_Pnt &pos, gp_Pnt &closest_pnt, double &u)const;
	bool GetLineParams(double *d6);
	bool GetCircleParams(double *d7);
	bool GetEllipseParams(double *d11);
	bool Orientation();
	double Length();
	double Length2(double uStart, double uEnd);
	void ReplaceVertex(ShapeBuild_ReShape& reshaper, const TopoDS_Vertex& vertex, const TopoDS_Vertex& new_vertex);
	HVertex* GetVertex0();
	HVertex* GetVertex1();
	CShape* GetParentBody();
};

