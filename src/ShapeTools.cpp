// ShapeTools.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Face.h"
#include "ShapeTools.h"
#include "Vertex.h"
#include <BRepAdaptor_Curve.hxx>

const wxBitmap &CFaceList::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/faces.png")));
	return *icon;
}

const wxBitmap &CEdgeList::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/edges.png")));
	return *icon;
}

const wxBitmap &CVertexList::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/vertices.png")));
	return *icon;
}

void CreateFacesAndEdges(TopoDS_Shape shape, CFaceList* faces, CEdgeList* edges, CVertexList* vertices)
{
	// create index maps
	TopTools_IndexedMapOfShape faceMap;
	TopTools_IndexedMapOfShape edgeMap;
	TopTools_IndexedMapOfShape vertexMap;
	for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
	{
		faceMap.Add(explorer.Current());
	}
	for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next())
	{
		edgeMap.Add(explorer.Current());
	}
	for (TopExp_Explorer explorer(shape, TopAbs_VERTEX); explorer.More(); explorer.Next())
	{
		vertexMap.Add(explorer.Current());
	}

	std::vector<CFace*> face_array;
	face_array.resize(faceMap.Extent() + 1);
	std::vector<CEdge*> edge_array;
	edge_array.resize(edgeMap.Extent() + 1);
	std::vector<HVertex*> vertex_array;
	vertex_array.resize(vertexMap.Extent() + 1);

	// create the edge objects
	for(int i = 1;i<=edgeMap.Extent();i++)
	{
		const TopoDS_Shape &s = edgeMap(i);
		CEdge* new_object = new CEdge(TopoDS::Edge(s));
		edge_array[i] = new_object;
	}

	// create the vertex objects
	for(int i = 1;i<=vertexMap.Extent();i++)
	{
		const TopoDS_Shape &s = vertexMap(i);
		HVertex* new_object = new HVertex(TopoDS::Vertex(s));
		vertex_array[i] = new_object;
	}

	// add the edges in their face loop order
	std::set<CEdge*> edges_added;
	std::set<HVertex*> vertices_added;

	// create the face objects
	for(int i = 1;i<=faceMap.Extent();i++)
	{
		const TopoDS_Shape &s = faceMap(i);
		CFace* new_face_object = new CFace(TopoDS::Face(s));
		faces->Add(new_face_object);
		face_array[i] = new_face_object;

		// create the loop objects
		TopTools_IndexedMapOfShape loopMap;
		for (TopExp_Explorer explorer(s, TopAbs_WIRE); explorer.More(); explorer.Next())
		{
			loopMap.Add(explorer.Current());
		}
		TopoDS_Wire outerWire=BRepTools::OuterWire(new_face_object->Face());
		int outer_index = loopMap.FindIndex(outerWire);
		for(int i = 1;i<=loopMap.Extent();i++)
		{
			const TopoDS_Shape &s = loopMap(i);
			CLoop* new_loop_object = new CLoop(TopoDS::Wire(s));
			new_face_object->m_loops.push_back(new_loop_object);
			if(outer_index == i)new_loop_object->m_is_outer = true;
			new_loop_object->m_pface = new_face_object;

			// find the loop's edges
			for(BRepTools_WireExplorer explorer(TopoDS::Wire(s)); explorer.More(); explorer.Next())
			{
				CEdge* e = edge_array[edgeMap.FindIndex(explorer.Current())];
				new_loop_object->m_edges.push_back(e);

				// add the edge
				if(edges_added.find(e) == edges_added.end())
				{
					edges->Add(e);
					edges_added.insert(e);
				}

				// add the vertex
				HVertex* v = vertex_array[vertexMap.FindIndex(explorer.CurrentVertex())];
				if(vertices_added.find(v) == vertices_added.end())
				{
					vertices->Add(v);
					vertices_added.insert(v);
				}
			}
		}
	}

	// find the vertices' edges
	for(unsigned int i = 1; i<vertex_array.size(); i++)
	{
		HVertex* v = vertex_array[i];
		TopTools_IndexedMapOfShape vertexEdgeMap;
		for (TopExp_Explorer expEdge(v->Vertex(), TopAbs_EDGE); expEdge.More(); expEdge.Next())
		{
			vertexEdgeMap.Add(expEdge.Current());
		}
		for(int i = 1; i<=vertexEdgeMap.Extent(); i++)
		{
			const TopoDS_Shape &s = vertexEdgeMap(i);
			CEdge* e = edge_array[edgeMap.FindIndex(s)];
			v->m_edges.push_back(e);
		}
	}

	// find the faces' edges
	for(unsigned int i = 1; i<face_array.size(); i++)
	{
		CFace* face = face_array[i];
		TopTools_IndexedMapOfShape faceEdgeMap;
		for (TopExp_Explorer expEdge(face->Face(), TopAbs_EDGE); expEdge.More(); expEdge.Next())
		{
			faceEdgeMap.Add(expEdge.Current());
		}
		for(int i = 1; i<=faceEdgeMap.Extent(); i++)
		{
			const TopoDS_Shape &s = faceEdgeMap(i);
			CEdge* e = edge_array[edgeMap.FindIndex(s)];
			face->m_edges.push_back(e);
			e->m_faces.push_back(face);
			bool sense = (s.IsEqual(e->Edge()) == Standard_True);
			e->m_face_senses.push_back(sense);
		}
	}
}


wxString TopoDS_ToString(const TopoDS_Shape& shape, int level)
{
    wxString rtn = (level == 0) ? "Shape:\n" : "";

    if (shape.IsNull())
    {
        rtn += "NULL\n";
        return rtn;
    }
    wxString indent('\t', level);
    wxString format;

    rtn += indent;
    rtn += "{";

    switch(shape.ShapeType())
    {
    case TopAbs_COMPOUND:
        rtn += "COMPOUND";
    break;

    case TopAbs_COMPSOLID:
        rtn += "COMPSOLID";
    break;

    case TopAbs_SOLID:
        rtn += "SOLID";
    break;

    case TopAbs_SHELL:
        rtn += "SHELL";
    break;

    case TopAbs_FACE:
    {
        rtn += "FACE";
        BRepAdaptor_Surface surface(TopoDS::Face(shape));
        switch(surface.GetType())
        {
        case GeomAbs_Plane:
            format = "type: plane";
        break;
        case GeomAbs_Cylinder:
            format = "type: cylinder";
        break;
        case GeomAbs_Cone:
            format = "type: cone";
        break;
        case GeomAbs_Sphere:
            format = "type: sphere";
        break;
        case GeomAbs_Torus:
            format = "type: torus";
        break;
        case GeomAbs_BezierSurface:
            format = "type: bezier_surface";
        break;
        case GeomAbs_BSplineSurface:
            format = "type: bspline_surface";
        break;
        case GeomAbs_SurfaceOfRevolution:
            format = "type: revolution_surface";
        break;
        case GeomAbs_SurfaceOfExtrusion:
            format = "type: extrusion_surface";
        break;
        case GeomAbs_OffsetSurface:
            format = "type: offset_surface";
        break;
        case GeomAbs_OtherSurface:
            format = "type: other_surface";
        break;
        }
    }
    break;

    case TopAbs_WIRE:
        rtn += "WIRE";
    break;
    case TopAbs_EDGE:
    {
        rtn += "EDGE";
        BRepAdaptor_Curve curve(TopoDS::Edge(shape));
        switch(curve.GetType())
        {
        case GeomAbs_Line:
            format = "type: line";
        break;
        case GeomAbs_Circle:
            format = "type: circle";
        break;
        case GeomAbs_Ellipse:
            format = "type: ellipse";
        break;
        case GeomAbs_Hyperbola:
            format = "type: hyperbola";
        break;
        case GeomAbs_Parabola:
            format = "type: parabola";
        break;
        case GeomAbs_BezierCurve:
            format = "type: bezier";
        break;
        case GeomAbs_BSplineCurve:
            format = "type: bspline";
        break;
        case GeomAbs_OtherCurve:
            format = "type: other";
        break;
        }
    }
    break;

    case TopAbs_VERTEX:
    {
        rtn += "VERTEX";
        gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(shape));
        format = wxString::Format("point: (%f, %f, %f)", pnt.X(), pnt.Y(), pnt.Z());
    }
    break;

    case TopAbs_SHAPE:
        rtn += "SHAPE";
    break;
    }

    TopAbs_Orientation orientation = shape.Orientation();
    rtn += ", closed: ";
    rtn += (shape.Closed() ? "true" : "false");
    rtn += ", orientation: ";
    rtn += ((orientation == TopAbs_FORWARD) ? "forward" :
            (orientation == TopAbs_REVERSED) ? "reversed" :
            (orientation == TopAbs_INTERNAL) ? "internal" :
            (orientation == TopAbs_EXTERNAL) ? "external" : "<unknown>");
    if (!format.IsEmpty())
        rtn += ", " + format;

    rtn += "}\n";

    for (TopoDS_Iterator iter2(shape); iter2.More(); iter2.Next())
    {
        // Recurse
        rtn += TopoDS_ToString(iter2.Value(), level + 1);
    }
    return rtn;
}
