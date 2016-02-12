// Vertex.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "Vertex.h"
#include "Face.h"
#include "Solid.h"
#include "Gripper.h"
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeBuild_ReShape.hxx>


HVertex::HVertex(const TopoDS_Vertex &vertex)
: HeeksObj(ObjType), m_topods_vertex(vertex)
{
    InitializeProperties();
    m_p = BRep_Tool::Pnt(vertex);
}

HVertex::~HVertex()
{
}

void HVertex::InitializeProperties()
{
    m_p.Initialize(_("Location"), this, true);
}

void HVertex::FindEdges()
{
	CShape* body = GetParentBody();
	if(body)
	{
		for(HeeksObj* object = body->m_edges->GetFirstChild(); object; object = body->m_edges->GetNextChild())
		{
			CEdge* e = (CEdge*)object;
			HVertex* v0 = e->GetVertex0();
			HVertex* v1 = e->GetVertex1();
			if(v0 == this || v1 == this)
			    m_edges.push_back(e);
		}
	}
}

const wxBitmap &HVertex::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)
	    icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/vertex.png")));
	return *icon;
}

void HVertex::glCommands(bool select, bool marked, bool no_color){
	// don't render anything, but put a point for selection
    double d[3];
    extract (m_p, d);
	glRasterPos3dv(d);
}

void HVertex::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	list->push_back(GripData(GripperTypeTranslate,m_p.X(),m_p.Y(),m_p.Z(),NULL));
}

void HVertex::ModifyByMatrix(const double *m)
{
    gp_Trsf mat = make_matrix ( m );
    m_p.Transform ( mat );
    TopoDS_Vertex new_vertex = BRepBuilderAPI_MakeVertex ( m_p );

    if ( this->GetOwner() == NULL )
    {
        m_topods_vertex = new_vertex;
        return;
    }

    ShapeBuild_ReShape reshaper;
    std::list<TopoDS_Face> added_faces;
    std::set<CFace*> modified_faces;
    for ( CEdge* edge = GetFirstEdge(); edge; edge = GetNextEdge() )
    {
        edge->ReplaceVertex ( reshaper, m_topods_vertex, new_vertex );
        for ( CFace* face = edge->GetFirstFace(); face; face = edge->GetNextFace() )
        {
            modified_faces.insert ( face );
        }
    }

    for ( std::set<CFace*>::iterator it = modified_faces.begin(); it != modified_faces.end(); it++ )
    {
        CFace* face = *it;
        face->ReplaceEdges ( reshaper, this, m_edges, added_faces );
    }

    CShape* parent = GetParentBody();
    HeeksObj * new_object = NULL;
    if (parent)
    {
        if ( added_faces.size() > 0 )
        {
            for (HeeksObj * object = GetParentBody()->m_faces->GetFirstChild(); object != NULL; object = GetParentBody()->m_faces->GetNextChild())
            {
                added_faces.push_back(TopoDS::Face(object->GetShape()));
            }
            new_object = CShape::SewFacesIntoShape ( added_faces );
            if ( new_object )
            {
                new_object->SetTitle(_("Modified Solid"));
                new_object->SetColor(parent->GetColor());
            }
        }
        else
        {
            new_object = CShape::MakeObject ( reshaper.Apply ( GetParentBody()->m_shape ), _("Modified solid"), SOLID_TYPE_UNKNOWN, parent->GetColor(), parent->GetOpacity() );
        }

        if ( new_object )
        {
            wxGetApp().Add ( new_object );
            wxGetApp().Remove ( GetParentBody() );
            wxGetApp().m_marked_list->Clear(true);
        }
    }
}

CEdge* HVertex::GetFirstEdge()
{
	if (m_edges.size()==0)
	    FindEdges();
	if (m_edges.size()==0)
	    return NULL;
	m_edgeIt = m_edges.begin();
	return *m_edgeIt;
}

CEdge* HVertex::GetNextEdge()
{
	if (m_edges.size()==0 || m_edgeIt==m_edges.end())
	    return NULL;
	m_edgeIt++;
	if (m_edgeIt==m_edges.end())
	    return NULL;
	return *m_edgeIt;
}

CShape* HVertex::GetParentBody()
{
    if ( this->GetOwner() == NULL )
        return NULL;
    if ( this->GetOwner()->GetOwner() == NULL )
        return NULL;
    if ( this->GetOwner()->GetOwner()->GetType() != SolidType )
        return NULL;
    return (CShape*) ( this->GetOwner()->GetOwner() );
}
