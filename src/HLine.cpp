// HLine.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"

#include "HLine.h"
#include "HILine.h"
#include "HCircle.h"
#include "HArc.h"
#include "../interface/Tool.h"
#include "Gripper.h"
#include "Sketch.h"
#include "Cylinder.h"
#include "Cone.h"
#include "DigitizeMode.h"
#include "Drawing.h"

HLine::HLine()
 : EndedObject(ObjType)
{
    InitializeProperties();
}

HLine::HLine(const HLine &line)
 : EndedObject(line)
{
	InitializeProperties();
    HeeksObj::operator=(line);      // my properties only
}

HLine::HLine(const gp_Pnt &a, const gp_Pnt &b, const HeeksColor& col)
 : EndedObject(ObjType, col)
{
	InitializeProperties();
	A->m_p = a;
	B->m_p = b;
}

HLine::~HLine()
{
}

const HLine& HLine::operator=(const HLine &b)
{
	EndedObject::operator=(b);
	return *this;
}

void HLine::InitializeProperties()
{
	m_start.Initialize(_("start"), this);
	m_start.SetTransient(true);
	m_end.Initialize(_("end"), this);
	m_end.SetTransient(true);
	m_length.Initialize(_("length"), this);
	m_length.SetReadOnly(true);
	m_length.SetTransient(true);
}

HLine* line_for_tool = NULL;

class MakeCylinderOnLine : public Tool
{
public:
	void Run()
	{
		gp_Vec v(line_for_tool->A->m_p, line_for_tool->B->m_p);
		CCylinder* new_object = new CCylinder(gp_Ax2(line_for_tool->A->m_p, v), 1.0, v.Magnitude(), _("Cylinder"), HeeksColor(191, 191, 240), 1.0f);
		wxGetApp().StartHistory();
		wxGetApp().Add(new_object);
		wxGetApp().EndHistory();
	}
	const wxChar* GetTitle(){return _("Make Cylinder On Line");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/cylonlin.png");}
};
static MakeCylinderOnLine make_cylinder_on_line;

class MakeConeOnLine:public Tool
{
public:
	void Run(){
		gp_Vec v(line_for_tool->A->m_p, line_for_tool->B->m_p);
		CCone* new_object = new CCone(gp_Ax2(line_for_tool->A->m_p, v), 2.0, 1.0, v.Magnitude(), _("Cone"), HeeksColor(240, 240, 191), 1.0f);
		wxGetApp().StartHistory();
		wxGetApp().Add(new_object);
		wxGetApp().EndHistory();
	}
	const wxChar* GetTitle(){return _("Make Cone On Line");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/coneonlin.png");}
};
static MakeConeOnLine make_cone_on_line;


class ClickMidpointOnLine:public Tool
{
public:
	void Run(){
		gp_Pnt midpoint((line_for_tool->A->m_p.XYZ() + line_for_tool->B->m_p.XYZ()) /2);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(midpoint, DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}
	const wxChar* GetTitle(){return _("Click Midpoint On Line");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_line_midpoint.png");}
};
static ClickMidpointOnLine click_midpoint_on_line;


class ClickStartPointOnLine:public Tool
{
public:
	void Run(){
		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(line_for_tool->A->m_p, DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}
	const wxChar* GetTitle(){return _("Click Start Of Line");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_line_end_one.png");}
};
static ClickStartPointOnLine click_start_point_on_line;


class ClickEndPointOnLine:public Tool
{
public:
	void Run(){
		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(line_for_tool->B->m_p, DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}
	const wxChar* GetTitle(){return _("Click End Of Line");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_line_end_two.png");}
};
static ClickEndPointOnLine click_end_point_on_line;



const wxBitmap &HLine::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/line.png")));
	return *icon;
}

bool HLine::GetMidPoint(double* pos)
{
	extract((A->m_p.XYZ() + B->m_p.XYZ())/2, pos);
	return true;
}

void HLine::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	line_for_tool = this;
	t_list->push_back(&make_cylinder_on_line);
	t_list->push_back(&make_cone_on_line);

	Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
	if (pDrawingMode != NULL)
	{
		t_list->push_back(&click_start_point_on_line);
		t_list->push_back(&click_midpoint_on_line);
		t_list->push_back(&click_end_point_on_line);
	}
}

void HLine::glCommands(bool select, bool marked, bool no_color){
	if(!no_color){
		wxGetApp().glColorEnsuringContrast(GetColor());
	}
	GLfloat save_depth_range[2];
	if(marked){
		glGetFloatv(GL_DEPTH_RANGE, save_depth_range);
		glDepthRange(0, 0);
		glLineWidth(2);
	}
	glBegin(GL_LINES);
	glVertex3d(A->m_p.X(), A->m_p.Y(), A->m_p.Z());
	glVertex3d(B->m_p.X(), B->m_p.Y(), B->m_p.Z());
	glEnd();
	if(marked){
		glLineWidth(1);
		glDepthRange(save_depth_range[0], save_depth_range[1]);
	}

	EndedObject::glCommands(select,marked,no_color);
}

void HLine::Draw(wxDC& dc)
{
	wxGetApp().PlotSetColor(GetColor());
	double s[3], e[3];
	extract(A->m_p, s);
	extract(B->m_p, e);
	wxGetApp().PlotLine(s, e);
}

HeeksObj *HLine::MakeACopy(void)const{
	HLine *new_object = new HLine(*this);
	return new_object;
}

void HLine::GetBox(CBox &box){
	box.Insert(A->m_p.X(), A->m_p.Y(), A->m_p.Z());
	box.Insert(B->m_p.X(), B->m_p.Y(), B->m_p.Z());
}

void HLine::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	EndedObject::GetGripperPositions(list,just_for_endof);
}

void HLine::OnPropertySet(Property& prop)
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

void HLine::GetProperties(std::list<Property *> *list)
{
	m_start = A->m_p;
	m_end = B->m_p;
	m_length = A->m_p.Distance(B->m_p);
	EndedObject::GetProperties(list);
}

bool HLine::FindNearPoint(const double* ray_start, const double* ray_direction, double *point){
	// The OpenCascade libraries throw an exception when one tries to
	// create a gp_Lin() object using a vector that doesn't point
	// anywhere.  If this is a zero-length line then we're in
	// trouble.  Don't bother with it.
	if ((A->m_p.X() == B->m_p.X()) &&
	    (A->m_p.Y() == B->m_p.Y()) &&
	    (A->m_p.Z() == B->m_p.Z())) return(false);

	gp_Lin ray(make_point(ray_start), make_vector(ray_direction));
	gp_Pnt p1, p2;
	ClosestPointsOnLines(GetLine(), ray, p1, p2);

	if(!Intersects(p1))
		return false;

	extract(p1, point);
	return true;
}

bool HLine::FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point){
	// any point on this line is a possible tangent point
	return FindNearPoint(ray_start, ray_direction, point);
}

gp_Lin HLine::GetLine()const{
	gp_Vec v(A->m_p, B->m_p);
	return gp_Lin(A->m_p, v);
}

int HLine::Intersects(const HeeksObj *object, std::list< double > *rl)const{
	int numi = 0;

	switch(object->GetType())
	{
    case SketchType:
        return( ((CSketch *)object)->Intersects( this, rl ));

	case LineType:
		{
			// The OpenCascade libraries throw an exception when one tries to
			// create a gp_Lin() object using a vector that doesn't point
			// anywhere.  If this is a zero-length line then we're in
			// trouble.  Don't bother with it.
			if ((A->m_p.X() == B->m_p.X()) &&
			    (A->m_p.Y() == B->m_p.Y()) &&
			    (A->m_p.Z() == B->m_p.Z())) break;

			gp_Pnt pnt;
			if(intersect(GetLine(), ((HLine*)object)->GetLine(), pnt))
			{
				if(Intersects(pnt) && ((HLine*)object)->Intersects(pnt)){
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
				if(Intersects(pnt)){
					if(rl)add_pnt_to_doubles(pnt, *rl);
					numi++;
				}
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
				if(Intersects(pnt) && ((HArc*)object)->Intersects(pnt))
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
			for(std::list<gp_Pnt>::iterator It = plist.begin(); It != plist.end(); It++)
			{
				gp_Pnt& pnt = *It;
				if(Intersects(pnt))
				{
					if(rl)add_pnt_to_doubles(pnt, *rl);
					numi++;
				}
			}
		}
		break;
	}

	return numi;
}

bool HLine::Intersects(const gp_Pnt &pnt)const
{
	gp_Lin this_line = GetLine();
	if(!intersect(pnt, this_line))return false;

	// check it lies between A and B
	gp_Vec v = this_line.Direction();
	double dpA = gp_Vec(A->m_p.XYZ()) * v;
	double dpB = gp_Vec(B->m_p.XYZ()) * v;
	double dp = gp_Vec(pnt.XYZ()) * v;
	return dp >= dpA - wxGetApp().m_geom_tol && dp <= dpB + wxGetApp().m_geom_tol;
}

void HLine::GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point)const{
	if(want_start_point)
	{
		double p[3];
		extract(A->m_p, p);
		(*callbackfunc)(p);
	}

	double p[3];
	extract(B->m_p, p);
	(*callbackfunc)(p);
}

gp_Vec HLine::GetSegmentVector(double fraction)
{
	gp_Vec line_vector(A->m_p, B->m_p);
	if(line_vector.Magnitude() < 0.000000001)return gp_Vec(0, 0, 0);
	return gp_Vec(A->m_p, B->m_p).Normalized();
}

// static member function
HeeksObj* HLine::ReadFromXMLElement(TiXmlElement* pElem)
{
    HLine* new_object = new HLine();
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
    else
    {
        new_object->A->m_p = new_object->m_start;
        new_object->B->m_p = new_object->m_end;
    }

    // The OpenCascade libraries throw an exception when one tries to
    // create a gp_Lin() object using a vector that doesn't point
    // anywhere.  If this is a zero-length line then we're in
    // trouble.  Don't bother with it.
    if (new_object->A->m_p.IsEqual(new_object->B->m_p, wxGetApp().m_geom_tol))
    {
        delete new_object;
        return(NULL);
    }

    return new_object;
}

void HLine::Reverse()
{
    gp_Pnt temp = A->m_p;
    A->m_p = B->m_p;
    B->m_p = temp;
}

