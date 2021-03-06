// HILine.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"

#include "HILine.h"
#include "HLine.h"
#include "HArc.h"
#include "HCircle.h"
#include "Gripper.h"

HILine::HILine()
 : EndedObject(ObjType)
{
    InitializeProperties();
}

HILine::HILine(const HILine &line)
 : EndedObject(line)
{
    InitializeProperties();
    HeeksObj::operator=(line);      // my properties only
}

HILine::HILine(const gp_Pnt &a, const gp_Pnt &b, const HeeksColor& col)
: EndedObject(ObjType, col)
{
	InitializeProperties();
	A->m_p = a;
	B->m_p = b;
}

HILine::~HILine()
{
}

const HILine& HILine::operator=(const HILine &b)
{
	EndedObject::operator=(b);
	return *this;
}

void HILine::InitializeProperties()
{
    m_start.Initialize(_("start"), this);
    m_start.SetTransient(true);
    m_end.Initialize(_("end"), this);
    m_end.SetTransient(true);
    m_length.Initialize(_("Length"), this);
    m_length.SetReadOnly(true);
    m_length.SetTransient(true);
}

const wxBitmap &HILine::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/iline.png")));
	return *icon;
}

void HILine::glCommands(bool select, bool marked, bool no_color)
{
	if(!no_color)
	{
		wxGetApp().glColorEnsuringContrast(GetColor());
		if (wxGetApp().m_allow_opengl_stippling)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(3, 0xaaaa);
		}
	}
	GLfloat save_depth_range[2];
	if(marked)
	{
		glGetFloatv(GL_DEPTH_RANGE, save_depth_range);
		glDepthRange(0, 0);
		glLineWidth(2);
	}

	gp_Vec v(A->m_p, B->m_p);
	if(v.Magnitude() > 0.0000000001)
	{
		v.Normalize();

		gp_Pnt p1 = A->m_p.XYZ() - v.XYZ() * 10000;
		gp_Pnt p2 = A->m_p.XYZ() + v.XYZ() * 10000;

		glBegin(GL_LINES);
		glVertex3d(p1.X(), p1.Y(), p1.Z());
		glVertex3d(p2.X(), p2.Y(), p2.Z());
		glEnd();
	}

	if(marked)
	{
		glLineWidth(1);
		glDepthRange(save_depth_range[0], save_depth_range[1]);
	}
	if(!no_color)
	{
		if (wxGetApp().m_allow_opengl_stippling)
		{
			glDisable(GL_LINE_STIPPLE);
		}
	}
}

HeeksObj *HILine::MakeACopy(void)const{
		HILine *new_object = new HILine(*this);
		return new_object;
}

void HILine::GetBox(CBox &box){
	box.Insert(A->m_p.X(), A->m_p.Y(), A->m_p.Z());
	box.Insert(B->m_p.X(), B->m_p.Y(), B->m_p.Z());
}

void HILine::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	if(!just_for_endof) // we don't want to snap to these for endof
	{
		EndedObject::GetGripperPositions(list,just_for_endof);
	}
}

void HILine::OnPropertySet(Property& prop)
{
    if ( prop == m_start )
    {
        A->m_p = m_start;
    }
    else if ( prop == m_end )
    {
        B->m_p = m_end;
    }
    else
    {
        EndedObject::OnPropertySet ( prop );
    }
}

void HILine::GetProperties(std::list<Property *> *list)
{
    m_start = A->m_p;
    m_end = B->m_p;
    m_length = A->m_p.Distance ( B->m_p );

    EndedObject::GetProperties ( list );
}

bool HILine::FindNearPoint(const double* ray_start, const double* ray_direction, double *point){
	gp_Lin ray(make_point(ray_start), make_vector(ray_direction));
	gp_Pnt p1, p2;
	ClosestPointsOnLines(GetLine(), ray, p1, p2);
	extract(p1, point);
	return true;
}

bool HILine::FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point){
	// any point on this line is a possible tangent point
	return FindNearPoint(ray_start, ray_direction, point);
}

gp_Lin HILine::GetLine()const{
	gp_Vec v(A->m_p,B->m_p);
	return gp_Lin(A->m_p, v);
}

int HILine::Intersects(const HeeksObj *object, std::list< double > *rl)const{
	int numi = 0;

	switch(object->GetType())
	{
    case SketchType:
        return( ((CSketch *)object)->Intersects( this, rl ));

	case LineType:
		{
			gp_Pnt pnt;
			if(intersect(GetLine(), ((HLine*)object)->GetLine(), pnt))
			{
				if(((HLine*)object)->Intersects(pnt)){
					if(rl)add_pnt_to_doubles(pnt, *rl);
					numi++;
				}
			}
		}
		break;

	case ILineType:
		{
			gp_Pnt pnt;
			if(intersect(GetLine(), ((HILine*)object)->GetLine(), pnt))
			{
				if(rl)add_pnt_to_doubles(pnt, *rl);
				numi++;
			}
		}
		break;

	case ArcType:
		{
			std::list<gp_Pnt> plist;
			intersect(GetLine(), ((HArc*)object)->GetCircle(), plist);
			for(std::list<gp_Pnt>::iterator It = plist.begin(); It != plist.end(); It++)
			{
				gp_Pnt& pnt = *It;
				if(((HArc*)object)->Intersects(pnt))
				{
					if(rl)add_pnt_to_doubles(pnt, *rl);
					numi++;
				}
			}
		}
		break;

	case CircleType:
		{
			std::list<gp_Pnt> plist;
			intersect(GetLine(), ((HCircle*)object)->GetCircle(), plist);
			if(rl)convert_pnts_to_doubles(plist, *rl);
			numi += plist.size();
		}
		break;
	}

	return numi;
}

bool HILine::GetStartPoint(double* pos)
{
	extract(A->m_p, pos);
	return true;
}

bool HILine::GetEndPoint(double* pos)
{
	extract(B->m_p, pos);
	return true;
}

// static member function
HeeksObj* HILine::ReadFromXMLElement(TiXmlElement* pElem)
{
	HILine* new_object = new HILine();
	new_object->ReadBaseXML(pElem);

	if(new_object->GetNumChildren()>2)
	{
		//This is a new style line, with children points
		new_object->Remove(new_object->A);
		new_object->Remove(new_object->B);
		delete new_object->A;
		delete new_object->B;
		new_object->A = (HPoint*)new_object->GetFirstChild();
		new_object->B = (HPoint*)new_object->GetNextChild();
		new_object->A->m_draw_unselected = false;
		new_object->B->m_draw_unselected = false;
	}
	return new_object;
}

