// Pyramid.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Gripper.h"
#include "MarkedList.h"
#include "Pyramid.h"
#include <BRepBuilderAPI_MakeSolid.hxx>


static TopoDS_Solid CreatePyramid(const gp_Ax2& pos, int sides, double len, double h)
{
    try {
        double angle = 2 * M_PI / sides;
        double circumradius = len / (2 * sin(M_PI/sides));

        const gp_Pnt& origin = pos.Location();
        gp_Pnt apex(origin.X(), origin.Y(), origin.Z() + h);
        gp_Pnt p1(origin.X(), origin.Y() + circumradius, origin.Z());

        BRepBuilderAPI_MakeWire wireMakerBase;
        BRepBuilderAPI_Sewing sewing(0.001);
        for (int i = 1; i <= sides; i++)
        {
            double x = origin.X() + circumradius * sin(i * angle);
            double y = origin.Y() + circumradius * cos(i * angle);
            gp_Pnt p2(x, y, origin.Z());
            BRepBuilderAPI_MakeWire wireMaker;
            wireMaker.Add(BRepBuilderAPI_MakeEdge (p1, apex));
            wireMaker.Add(BRepBuilderAPI_MakeEdge (p1, p2));
            wireMaker.Add(BRepBuilderAPI_MakeEdge (p2, apex));
            wireMakerBase.Add(BRepBuilderAPI_MakeEdge (p1, p2));
            TopoDS_Wire wire = wireMaker.Wire();
            BRepBuilderAPI_MakeFace faceMaker(wire);
            TopoDS_Face face = faceMaker.Face();
            sewing.Add(face);
            p1 = p2;
        }
        BRepBuilderAPI_MakeFace faceMaker(wireMakerBase.Wire());
        TopoDS_Face base = faceMaker.Face();
        sewing.Add(base);
        sewing.Perform();
        BRepBuilderAPI_MakeSolid solid_maker;
        solid_maker.Add(TopoDS::Shell(sewing.SewedShape()));
        return solid_maker.Solid();
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        wxMessageBox(wxString(_("Error making pyramid")) + _T(": ") + Ctt(e->GetMessageString()));
    }
    return TopoDS_Solid();
}

CPyramid::CPyramid(const gp_Ax2& pos, int sides, double length, double height, const wxChar* title, const HeeksColor& col, float opacity)
 : CSolid(CreatePyramid(pos, sides, length, height), title, col, opacity),
 m_pos(pos), m_sides(sides), m_length(length), m_height(height)
{
	InitializeProperties();
}

CPyramid::CPyramid(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity)
 : CSolid(solid, title, col, opacity),
 m_pos(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)))
{
	InitializeProperties();
}

CPyramid::CPyramid( const CPyramid & rhs )
 : CSolid(rhs)
{
    InitializeProperties();
    operator=(rhs);
}

const wxBitmap &CPyramid::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/pyramid.png")));
	return *icon;
}

HeeksObj *CPyramid::MakeACopy(void)const
{
	return new CPyramid(*this);
}

CPyramid & CPyramid::operator= ( const CPyramid & rhs )
{
    CSolid::operator=( rhs );
    return(*this);
}

void CPyramid::InitializeProperties()
{
    m_pos.Initialize(_("coordinates"), this, true);
    m_sides.Initialize(_("sides"), this, true);
    m_length.Initialize(_("base edge length"), this, true);
    m_height.Initialize(_("height ( z )"), this, true);
}

bool CPyramid::IsDifferent(HeeksObj* other)
{
    CPyramid* pyramid = (CPyramid*)other;
	if(pyramid->m_sides != m_sides || pyramid->m_length != m_length || pyramid->m_height != m_height)
		return true;

	if(!IsEqual(pyramid->m_pos, m_pos))
		return true;

	return CShape::IsDifferent(other);
}


void CPyramid::MakeTransformedShape(const gp_Trsf &mat)
{
	m_pos.Transform(mat);
	double scale = gp_Vec(1, 0, 0).Transformed(mat).Magnitude();
	m_length = fabs(m_length * scale);
	m_height = fabs(m_height * scale);
	CSolid::MakeTransformedShape(mat);
}

wxString CPyramid::StretchedName(){ return _("Stretched Pyramid");}

void CPyramid::GetProperties(std::list<Property *> *list)
{
//	CoordinateSystem::GetAx2Properties(list, m_pos);
	CSolid::GetProperties(list);
}

void CPyramid::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	gp_Pnt o = m_pos.Location();
	list->push_back(GripData(GripperTypeTranslate,o.X(),o.Y(),o.Z(),NULL));
}

void CPyramid::OnPropertySet(Property& prop)
{
    if (in_set) {
        return;
    }
	if (prop == m_pos || prop == m_sides || prop == m_length || prop == m_height) {
	    m_shape = CreatePyramid(m_pos, m_sides, m_length, m_height);
        delete_faces_and_edges();
        KillGLLists();
        create_faces_and_edges();
	}
	else {
	    CSolid::OnPropertySet(prop);
	}
}

bool CPyramid::GetScaleAboutMatrix(double *m)
{
	gp_Trsf mat = make_matrix(m_pos.Location(), m_pos.XDirection(), m_pos.YDirection());
	extract(mat, m);
	return true;
}


void CPyramid::SetXMLElement(TiXmlElement* element)
{
    const gp_Pnt& l = m_pos.Location();
    element->SetDoubleAttribute("lx", l.X());
    element->SetDoubleAttribute("ly", l.Y());
    element->SetDoubleAttribute("lz", l.Z());

    const gp_Dir& d = m_pos.Direction();
    element->SetDoubleAttribute("dx", d.X());
    element->SetDoubleAttribute("dy", d.Y());
    element->SetDoubleAttribute("dz", d.Z());

    const gp_Dir& x = m_pos.XDirection();
    element->SetDoubleAttribute("xx", x.X());
    element->SetDoubleAttribute("xy", x.Y());
    element->SetDoubleAttribute("xz", x.Z());

    element->SetAttribute("sides", m_sides);
    element->SetDoubleAttribute("length", m_length);
    element->SetDoubleAttribute("height", m_height);

    CSolid::SetXMLElement(element);
}

void CPyramid::SetFromXMLElement(TiXmlElement* pElem)
{
    double l[3] = {0, 0, 0};
    double d[3] = {0, 0, 1};
    double x[3] = {1, 0, 0};

    in_set = true;
    for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
    {
        std::string name(a->Name());
        if(name == "lx")     {l[0] = a->DoubleValue();}
        else if(name == "ly"){l[1] = a->DoubleValue();}
        else if(name == "lz"){l[2] = a->DoubleValue();}

        else if(name == "dx"){d[0] = a->DoubleValue();}
        else if(name == "dy"){d[1] = a->DoubleValue();}
        else if(name == "dz"){d[2] = a->DoubleValue();}

        else if(name == "xx"){x[0] = a->DoubleValue();}
        else if(name == "xy"){x[1] = a->DoubleValue();}
        else if(name == "xz"){x[2] = a->DoubleValue();}

        else if(name == "sides"){m_sides = a->IntValue();}
        else if(name == "length"){m_length = a->DoubleValue();}
        else if(name == "height"){m_height = a->DoubleValue();}
    }
    m_pos = gp_Ax2(make_point(l), make_vector(d), make_vector(x));

    in_set = false;
    CSolid::SetFromXMLElement(pElem);
}
