// HSpline.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"

#include "HSpline.h"
#include "../tinyxml/tinyxml.h"
#include "HLine.h"
#include "HILine.h"
#include "HArc.h"
#include "Gripper.h"

CTangentialArc::CTangentialArc(HeeksObj * owner, const gp_Pnt &p0, const gp_Vec &v0, const gp_Pnt &p1)
 : m_owner(owner), m_p0(p0), m_v0(v0), m_p1(p1)
{
	// calculate a tangential arc that goes through p0 and p1, with a direction of v0 at p0
	m_is_a_line = !HArc::TangentialArc(m_p0, m_v0, m_p1, m_c, m_a);
}

bool CTangentialArc::radius_equal(const gp_Pnt &p, double tolerance) const
{
	if(m_is_a_line)
	    return 0.0;

	double point_radius = gp_Vec(m_c.XYZ() - p.XYZ()).Magnitude();
	double diff =  fabs(point_radius - radius());

	return diff <= tolerance;
}

double CTangentialArc::radius() const
{
	double r0 = gp_Vec(m_p0.XYZ() - m_c.XYZ()).Magnitude();
	double r1 = gp_Vec(m_p1.XYZ() - m_c.XYZ()).Magnitude();
	double r = (r0 + r1)/2;
	return r;
}

HeeksObj* CTangentialArc::MakeHArc() const
{
	gp_Circ c(gp_Ax2(m_c, m_a), radius());
	HArc* new_object = new HArc(m_p0, m_p1, c, wxGetApp().CurrentColor());
	new_object->SetOwner(m_owner);
	return new_object;
}

HSpline::HSpline()
 : EndedObject(ObjType), spline_created(false)
{
    InitializeProperties();
}

HSpline::HSpline(const HSpline &s)
 : EndedObject(s)
{
	InitializeProperties();
    m_spline = Handle(Geom_BSplineCurve)::DownCast((s.m_spline)->Copy());
    spline_created = true;
    gp_Pnt a;
    gp_Pnt b;
    m_spline->D0(m_spline->FirstParameter(), a);
    m_spline->D0(m_spline->LastParameter(), b);
    A->m_p = a;
    B->m_p = b;
}

HSpline::HSpline(const Geom_BSplineCurve &s, const HeeksColor& col)
 : EndedObject(ObjType, col)
{
	InitializeProperties();
	m_spline = Handle(Geom_BSplineCurve)::DownCast(s.Copy());
	spline_created = true;
    gp_Pnt a;
    gp_Pnt b;
    m_spline->D0(m_spline->FirstParameter(), a);
    m_spline->D0(m_spline->LastParameter(), b);
    A->m_p = a;
    B->m_p = b;
}

HSpline::HSpline(Handle_Geom_BSplineCurve s, const HeeksColor& col)
 : EndedObject(ObjType, col)
{
	InitializeProperties();
	m_spline = s;
	spline_created = true;
    gp_Pnt a;
    gp_Pnt b;
    m_spline->D0(m_spline->FirstParameter(), a);
    m_spline->D0(m_spline->LastParameter(), b);
    A->m_p = a;
    B->m_p = b;
}

HSpline::HSpline(const std::list<gp_Pnt> &points, const HeeksColor& col)
 : EndedObject(ObjType, col)
{
	InitializeProperties();
	Standard_Boolean periodicity = points.front().IsEqual(points.back(), wxGetApp().m_geom_tol);

	unsigned int size = points.size();
	if(periodicity == Standard_True)
	    size--;

	TColgp_HArray1OfPnt *Array = new TColgp_HArray1OfPnt(1, size);

	unsigned int i = 1;
	for(std::list<gp_Pnt>::const_iterator It = points.begin(); i <= size; It++, i++)
	{
		Array->SetValue(i, *It);
	}

	GeomAPI_Interpolate anInterpolation(Array, periodicity, Precision::Approximation());
	anInterpolation.Perform();
	m_spline = anInterpolation.Curve();
	spline_created = true;
	gp_Pnt a;
	gp_Pnt b;
	m_spline->D0(m_spline->FirstParameter(), a);
	m_spline->D0(m_spline->LastParameter(), b);
	A->m_p = a;
	B->m_p = b;
}

HSpline::~HSpline()
{
}

const HSpline& HSpline::operator=(const HSpline &s)
{
	EndedObject::operator=(s);
	m_spline = Handle(Geom_BSplineCurve)::DownCast((s.m_spline)->Copy());
	spline_created = true;
	return *this;
}

void HSpline::InitializeProperties()
{
    m_degree.Initialize(_("degree"), this);
    m_degree.SetReadOnly(true);
    m_rational.Initialize(_("rational"), this);
    m_rational.SetReadOnly(true);
    m_periodic.Initialize(_("periodic"), this);
    m_periodic.SetReadOnly(true);
	m_knots.Initialize(_("knots"), this);
	m_knots.SetReadFromXmlFunction(&ReadKnotsXML);
	m_poles.Initialize(_("poles"), this);
    m_poles.SetReadFromXmlFunction(&ReadPolesXML);
}

const wxBitmap &HSpline::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/spline.png")));
	return *icon;
}

bool HSpline::IsDifferent(HeeksObj* o)
{
	HSpline* other = (HSpline*)o;
	for(int i=1; i <= m_spline->NbPoles(); i++)
	{
		if(m_spline->Pole(i).Distance(other->m_spline->Pole(i))>wxGetApp().m_geom_tol)
			return true;
	}

	return EndedObject::IsDifferent(o);
}

//segments - number of segments per full revolution!
void HSpline::GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point)const
{
	//TODO: calculate length
	double u0 = m_spline->FirstParameter();
	double u1 = m_spline->LastParameter();
	double uw = u1 - u0;

	gp_Pnt p0, phalf, p1;
	m_spline->D0(u0, p0);
	m_spline->D0(u0 + uw*0.5, phalf);
	m_spline->D0(u1, p1);
	double approx_length = p0.Distance(phalf) + phalf.Distance(p1);

	int segments = (int)(fabs(pixels_per_mm * approx_length + 1));
	double pp[3];

	for(int i = 0; i <= segments; i++)
    {
		gp_Pnt p;
		m_spline->D0(u0 + ((double)i / segments) * uw,p);
		extract(p, pp);
		(*callbackfunc)(pp);
    }
}

static void glVertexFunction(const double *p){glVertex3d(p[0], p[1], p[2]);}

void HSpline::glCommands(bool select, bool marked, bool no_color)
{
	if(!no_color)
	{
		wxGetApp().glColorEnsuringContrast(GetColor());
//		if (wxGetApp().m_allow_opengl_stippling)
//		{
//			glEnable(GL_LINE_STIPPLE);
//			glLineStipple(3, 0xaaaa);
//		}
	}
	GLfloat save_depth_range[2];
	if(marked){
		glGetFloatv(GL_DEPTH_RANGE, save_depth_range);
		glDepthRange(0, 0);
		glLineWidth(2);
	}

	glBegin(GL_LINE_STRIP);
	GetSegments(glVertexFunction, wxGetApp().GetPixelScale());
	glEnd();

	if(marked)
	{
		glLineWidth(1);
		glDepthRange(save_depth_range[0], save_depth_range[1]);
	}
	if(!no_color)
	{
//		if (wxGetApp().m_allow_opengl_stippling)
//		{
//			glDisable(GL_LINE_STIPPLE);
//		}
	}

    EndedObject::glCommands(select,marked,no_color);
}

HeeksObj *HSpline::MakeACopy(void) const
{
    HSpline *new_object = new HSpline(*this);
    return new_object;
}

void HSpline::ModifyByMatrix ( const double* m )
{
    gp_GTrsf gm = make_general_matrix ( m );

    for ( int i = 1; i <= m_spline->NbPoles ( ); i++ )
    {
        gp_XYZ pole = m_spline->Pole ( i ).XYZ ( );
        gm.Transforms ( pole );
        {
            m_spline->SetPole ( i, gp_Pnt ( pole ) );
        }
    }
}

void HSpline::GetBox(CBox &box)
{
	//TODO: get rid of magic number
	double pp[3];
	double u0 = m_spline->FirstParameter();
	double u1 = m_spline->LastParameter();
	double uw = u1 - u0;

	for(int i = 0; i <= 100; i++)
    {
		gp_Pnt p;
		m_spline->D0(u0 + uw * .01,p);
		extract(p, pp);
		box.Insert(pp);
    }
}

void HSpline::RedistributePoles()
{
    int size = m_spline->NbPoles() * 2;
    TColgp_HArray1OfPnt *Array = new TColgp_HArray1OfPnt(1, size);

    double u0 = m_spline->FirstParameter();
    double u1 = m_spline->LastParameter();
    double delta = (u1 - u0) / (size - 1);

    for(unsigned int i = 1; i <= size; i++)
    {
        gp_Pnt p;
        m_spline->D0(u0,p);
        Array->SetValue(i, p);
        u0 += delta;
    }

    GeomAPI_Interpolate anInterpolation(Array, m_spline->IsPeriodic(), wxGetApp().m_geom_tol);
    anInterpolation.Perform();
    m_spline = anInterpolation.Curve();
}

void HSpline::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	if(!just_for_endof)
	{
		for(int i=1; i <= m_spline->NbPoles(); i++)
		{
			gp_Pnt pole = m_spline->Pole(i);
			list->push_back(GripData(GripperTypeStretch,pole.X(),pole.Y(),pole.Z(),NULL, true));
		}
	}
}

void HSpline::OnPropertySet(Property& prop)
{
    wxString name = prop.GetName ( );
    wxString rest;
    int i;
    if ( name.StartsWith ( "knot_", &rest ) )
    {
        i = wxAtoi ( rest );
        m_spline->SetKnot ( i, (const PropertyDouble&) prop );
    }
    else if ( name.StartsWith ( "pole_", &rest ) )
    {
        i = wxAtoi ( rest );
        m_spline->SetPole ( i, (const PropertyVertex&) prop );
    }
    else if ( name.StartsWith ( "weight_", &rest ) )
    {
        i = wxAtoi ( rest );
        m_spline->SetWeight ( i, (const PropertyDouble&) prop );
    }
    else
    {
        EndedObject::OnPropertySet ( prop );
    }
}

void HSpline::GetProperties(std::list<Property *> *list)
{
    if (spline_created)
    {
        wxString name;
        wxString title;

        m_degree = m_spline->Degree();
        m_rational = m_spline->IsRational();
        m_periodic = m_spline->IsPeriodic();

        m_knots.Clear();
        for(int i=1; i <= m_spline->NbKnots(); i++)
        {
            PropertyDouble * knot;
            name.Printf("%s_%d", _("knot"), i);
            title.Printf("%s %d", _("Knot"), i);
            knot = new PropertyDouble(name, title, NULL);
            *knot = m_spline->Knot(i);
            m_knots.AddProperty(knot);

            PropertyInt * mult;
            name.Printf("%s_%d", _("multiplicity"), i);
            title.Printf("%s %d", _("Multiplicity"), i);
            mult = new PropertyInt(name, title, NULL);
            *mult = m_spline->Multiplicity(i);
            mult->SetReadOnly(true);
            m_knots.AddProperty(mult);
        }

        m_poles.Clear();
        for(int i=1; i <= m_spline->NbPoles(); i++)
        {
            PropertyVertex * pole;
            name.Printf("%s_%d", _("pole"), i);
            title.Printf("%s %d", _("Pole"), i);
            pole = new PropertyVertex(name, title, NULL);
            *pole = m_spline->Pole(i);
            m_poles.AddProperty(pole);

            if (m_rational.IsSet()) {
                PropertyDouble * weight;
                name.Printf("%s_%d", _("weight"), i);
                title.Printf("%s %d", _("Weight"), i);
                weight = new PropertyDouble(name, title, NULL);
                *weight = m_spline->Weight(i);
                m_poles.AddProperty(weight);
            }
        }
    }

	EndedObject::GetProperties(list);
}

bool HSpline::FindNearPoint(const double* ray_start, const double* ray_direction, double *point){
	gp_Lin ray(make_point(ray_start), make_vector(ray_direction));
	std::list< gp_Pnt > rl;

	return false;
}

bool HSpline::FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point){
	// any point on this ellipse is a possible tangent point
	return FindNearPoint(ray_start, ray_direction, point);
}

bool HSpline::Stretch(const double *p, const double* shift, void* data){

	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);

	//There may be multiple control points at the same location, so we must move them all
	// This could break if user drags one set of points over another
	// The correct mapping of handles to control points can be determined
	// from the multiplicity set
	for(int i = 1; i <= m_spline->NbPoles(); i++)
	{
		if(m_spline->Pole(i).IsEqual(vp, wxGetApp().m_geom_tol))
		{
			m_spline->SetPole(i,m_spline->Pole(i).XYZ() + vshift.XYZ());
		}
	}
	return false;
}

// static member function
void HSpline::ReadKnotsXML(Property * prop, TiXmlElement *element)
{
    // Fill in the lists with the varying number of properties.
    wxString name;
    wxString title;
    wxString rest;
    long int index;

    for ( TiXmlElement* c = element->FirstChildElement ( ); c; c = c->NextSiblingElement ( ) )
    {
        name = c->Value ( );
        if (name.StartsWith("knot_", &rest) && rest.ToLong(&index, 10))
        {
            PropertyList * proplist = (PropertyList *)prop;
            PropertyDouble * knot;
            title.Printf("%s %ld", _("Knot"), index);
            knot = new PropertyDouble(name, title, NULL);
            knot->ReadFromXmlElement(c);
            proplist->AddProperty(knot);
        }
        else if (name.StartsWith("multiplicity_", &rest) && rest.ToLong(&index, 10))
        {
            PropertyList * proplist = (PropertyList *)prop;
            PropertyInt * mult;
            title.Printf("%s %ld", _("Multiplicity"), index);
            mult = new PropertyInt(name, title, NULL);
            mult->ReadFromXmlElement(c);
            mult->SetReadOnly(true);
            proplist->AddProperty(mult);
        }
    }
}

// static member function
void HSpline::ReadPolesXML(Property * prop, TiXmlElement *element)
{
    // Fill in the lists with the varying number of properties.
    wxString name;
    wxString title;
    wxString rest;
    long int index;

    for ( TiXmlElement* c = element->FirstChildElement ( ); c; c = c->NextSiblingElement ( ) )
    {
        name = c->Value ( );
        if (name.StartsWith("pole_", &rest) && rest.ToLong(&index, 10))
        {
            PropertyList * proplist = (PropertyList *)prop;
            PropertyVertex * pole;
            title.Printf("%s %ld", _("Pole"), index);
            pole = new PropertyVertex(name, title, NULL);
            pole->ReadFromXmlElement(c);
            proplist->AddProperty(pole);
        }
        else if (name.StartsWith("weight_", &rest) && rest.ToLong(&index, 10))
        {
            PropertyList * proplist = (PropertyList *)prop;
            PropertyDouble * weight;
            title.Printf("%s %ld", _("Weight"), index);
            weight = new PropertyDouble(name, title, NULL);
            weight->ReadFromXmlElement(c);
            proplist->AddProperty(weight);
        }
    }
}

// static member function
HeeksObj* HSpline::ReadFromXMLElement(TiXmlElement* pElem)
{
    HSpline* new_object = new HSpline();
    new_object->ReadBaseXML(pElem);

    if ( new_object->m_degree == 0 )
        new_object->m_degree = 3;

    int nknots = 0;
    int npoles = 0;

    DomainObjectIterator iterator;
    for (iterator = new_object->m_knots.begin(); iterator != new_object->m_knots.end(); iterator++)
    {
        Property * prop = *iterator;
        const wxString& name = prop->GetName();
        if (name.StartsWith("knot_"))
        {
            nknots++;
        }
    }

    for (iterator = new_object->m_poles.begin(); iterator != new_object->m_poles.end(); iterator++)
    {
        Property * prop = *iterator;
        const wxString& name = prop->GetName();
        if (name.StartsWith("pole_"))
        {
            npoles++;
        }
    }

    TColgp_Array1OfPnt tkcontrol (1, npoles);
    TColStd_Array1OfReal tkweight (1, npoles);
    TColStd_Array1OfReal tkknot (1, nknots);
    TColStd_Array1OfInteger tkmult (1, nknots);

    for (int i = 1; i <= npoles; i++)
    {
        tkweight.SetValue(i, 1.0);
    }

    wxString rest;
    long int index;

    // get the children
    for (iterator = new_object->m_knots.begin(); iterator != new_object->m_knots.end(); iterator++)
    {
        Property * prop = *iterator;
        const wxString& name = prop->GetName();

        if (name.StartsWith("knot_", &rest) && rest.ToLong(&index, 10))
        {
            double value = *(PropertyDouble *)prop;
            tkknot.SetValue(index, value);
        }
        else if (name.StartsWith("multiplicity_", &rest) && rest.ToLong(&index, 10))
        {
            int value = *(PropertyInt *)prop;
            tkmult.SetValue(index, value);
        }
    }

    for (iterator = new_object->m_poles.begin(); iterator != new_object->m_poles.end(); iterator++)
    {
        Property * prop = *iterator;
        const wxString& name = prop->GetName();

        if (name.StartsWith("pole_", &rest) && rest.ToLong(&index, 10))
        {
            gp_Pnt value = *(PropertyVertex *)prop;
            tkcontrol.SetValue(index, value);
        }
        else if (name.StartsWith("weight_", &rest) && rest.ToLong(&index, 10))
        {
            double value = *(PropertyDouble *)prop;
            tkweight.SetValue(index, value);
        }
    }

    Geom_BSplineCurve spline(tkcontrol, tkweight, tkknot, tkmult,
                             new_object->m_degree, new_object->m_periodic, new_object->m_rational);
    new_object->m_spline = Handle(Geom_BSplineCurve)::DownCast(spline.Copy());
    new_object->spline_created = true;

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

    gp_Pnt a;
    gp_Pnt b;
    new_object->m_spline->D0(new_object->m_spline->FirstParameter(), a);
    new_object->m_spline->D0(new_object->m_spline->LastParameter(), b);
    new_object->A->m_p = a;
    new_object->B->m_p = b;

    return new_object;
}

int HSpline::Intersects(const HeeksObj *object, std::list< double > *rl)const
{
/*	int numi = 0;

	switch(object->GetType())
	{
	case LineType:
		{
			std::list<gp_Pnt> plist;
			intersect(((HLine*)object)->GetLine(), m_spline, plist);
			for(std::list<gp_Pnt>::iterator It = plist.begin(); It != plist.end(); It++)
			{
				gp_Pnt& pnt = *It;
				if(((HLine*)object)->Intersects(pnt))
				{
					if(rl)add_pnt_to_doubles(pnt, *rl);
					numi++;
				}
			}
		}
		break;

	case ILineType:
		{
			std::list<gp_Pnt> plist;
			intersect(((HILine*)object)->GetLine(), m_spline, plist);
			if(rl)convert_pnts_to_doubles(plist, *rl);
			numi += plist.size();
		}
		break;

	case ArcType:
		{
			std::list<gp_Pnt> plist;
			intersect(m_spline, ((HArc*)object)->m_circle, plist);
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
			intersect(m_spline, ((HSpline*)object)->m_circle, plist);
			if(rl)convert_pnts_to_doubles(plist, *rl);
			numi += plist.size();
		}
		break;
	}
*/
	return 0; //return numi;
}

static bool calculate_biarc_points(const gp_Pnt &p_start, gp_Vec v_start,
                                   const gp_Pnt &p_end, gp_Vec v_end,
                                   gp_Pnt &p1, gp_Pnt &p2, gp_Pnt &p3)
{
    if (IsZeroVector(v_start) || IsZeroVector(v_end)) {
        return false;
    }
    v_start.Normalize();
    v_end.Normalize();

    gp_Vec v = p_start.XYZ() - p_end.XYZ();

    double a = 2*(v_start*v_end-1);
    double b = (v*2)*(v_start+v_end);
    double c = v*v;

    if(fabs(a) < 0.000000000000001)
        return false;

    double d = b*b-4*a*c;

    if(d < 0)
        return false;

    double sd = sqrt(d);

    double e1 = (-b - sd) / (2.0 * a);
    double e2 = (-b + sd) / (2.0 * a);

    if(e1 > 0 && e2 > 0)
        return false;

    double e = (e2 > e1) ? e2 : e1;

    if(e < 0)
        return false;

    p1 = p_start.XYZ() + v_start.XYZ() * e;
    p3 = p_end.XYZ() - v_end.XYZ() * e;
    p2 = p1.XYZ() * 0.5 + p3.XYZ() * 0.5;

    return true;
}

static std::list<HeeksObj*>* new_spans_for_CreateArcs = NULL;
static double tolerance_for_CreateArcs = 1.0;
static Handle(Geom_BSplineCurve) spline_for_CreateArcs = NULL;

void CreateArcs(HeeksObj * owner, const gp_Pnt &p_start, const gp_Vec &v_start, double t_start, double t_end, gp_Pnt &p_end, gp_Vec &v_end)
{
	spline_for_CreateArcs->D1(t_end, p_end, v_end);

	if (fabs(v_end.X()) < 0.000000000000001 &&
	    fabs(v_end.Y()) < 0.000000000000001 &&
	    fabs(v_end.Z()) < 0.000000000000001) {
	    spline_for_CreateArcs->D1(t_end, p_end, v_end);
	}
	gp_Pnt p1, p2, p3;

	bool can_do_spline_whole = calculate_biarc_points(p_start, v_start, p_end, v_end, p1, p2, p3);

	HeeksObj* arc_object1 = NULL;
	HeeksObj* arc_object2 = NULL;

	if(can_do_spline_whole)
	{
		CTangentialArc arc1(owner, p_start, v_start, p2);
		CTangentialArc arc2(owner, p2, gp_Vec(p3.XYZ() - p2.XYZ()), p_end);

		gp_Pnt p_middle1, p_middle2;

        spline_for_CreateArcs->D0(t_start + ((t_end - t_start) * 0.25), p_middle1);
        spline_for_CreateArcs->D0(t_start + ((t_end - t_start) * 0.75), p_middle2);

		if(!arc1.radius_equal(p_middle1, tolerance_for_CreateArcs) ||
		   !arc2.radius_equal(p_middle2, tolerance_for_CreateArcs))
		{
			can_do_spline_whole = false;
		}
		else
		{
			arc_object1 = arc1.MakeHArc();
			arc_object2 = arc2.MakeHArc();
		}
	}
	else
	{
		// calculate_biarc_points failed, just add a line
		new_spans_for_CreateArcs->push_back(new HLine(p_start, p_end, wxGetApp().CurrentColor()));
		return;
	}

	if(can_do_spline_whole)
	{
		new_spans_for_CreateArcs->push_back(arc_object1);
		new_spans_for_CreateArcs->push_back(arc_object2);
	}
	else
	{
		double t_middle = t_start + ((t_end - t_start) * 0.5);
		gp_Pnt p_middle;
		gp_Vec v_middle;
		CreateArcs(owner, p_start, v_start, t_start, t_middle, p_middle, v_middle);// recursive
		gp_Pnt new_p_end;
		gp_Vec new_v_end;
		CreateArcs(owner, p_middle, v_middle, t_middle, t_end, new_p_end, new_v_end);
	}
}

bool HSpline::GetStartPoint(double* pos)
{
	gp_Pnt p;
	m_spline->D0(m_spline->FirstParameter(), p);
	extract(p, pos);
	return true;
}

bool HSpline::GetEndPoint(double* pos)
{
	gp_Pnt p;
	m_spline->D0(m_spline->LastParameter(), p);
	extract(p, pos);
	return true;
}

void HSpline::ToBiarcs(HeeksObj * owner, const Handle_Geom_BSplineCurve s, std::list<HeeksObj*> &new_spans, double tolerance, double first_parameter, double last_parameter)
{
	new_spans_for_CreateArcs = &new_spans;
	if(tolerance < 0.000000000000001)
	    tolerance = 0.000000000000001;
	tolerance_for_CreateArcs = tolerance;
	gp_Pnt p_start;
	gp_Vec v_start;
	gp_Pnt p_end;
	gp_Vec v_end;
	s->D1(first_parameter, p_start, v_start);
	spline_for_CreateArcs = s;
	try {
	    CreateArcs(owner, p_start, v_start, first_parameter, last_parameter, p_end, v_end);
	}
	catch(Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        wxMessageBox(wxString(("Error making arcs from spline")) + _T(": ") + Ctt(e->GetMessageString()));
    }
}

void HSpline::ToBiarcs(std::list<HeeksObj*> &new_spans, double tolerance)
{
    gp_Pnt polepnt = m_spline->Pole(1);
    for (int i=2; i <= m_spline->NbPoles(); i++)
    {
        gp_Pnt cursor = m_spline->Pole(i);
        if (cursor.IsEqual(polepnt, wxGetApp().m_geom_tol)) {
            RedistributePoles();
            break;
        }
        polepnt = cursor;
    }

	ToBiarcs((HeeksObj *)this, m_spline, new_spans, tolerance, m_spline->FirstParameter(), m_spline->LastParameter());
}

void HSpline::Reverse()
{
	m_spline->Reverse();
}

static HSpline* spline_for_tools = NULL;

class IncreaseDegreeTool : public Tool{
public:
    // Tool's virtual functions
    void Run() {
        int degree = spline_for_tools->m_spline->Degree();
        spline_for_tools->m_spline->IncreaseDegree(degree+1);
    }
    const wxChar* GetTitle(){ return _("Increase Degree (Add Point)");}
    wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/offsetsolid.png");}
};

static IncreaseDegreeTool increase_degree_tool;

void HSpline::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
    spline_for_tools = this;
    t_list->push_back(&increase_degree_tool);
}


wxString HSpline::ToString() const
{
    wxString rtn;
    wxString buf;
    rtn = "HSpline: {";
    for (int i=1; i <= m_spline->NbPoles(); i++)
    {
        if (i > 1)
        {
            rtn += ", ";
        }
        gp_Pnt polepnt = m_spline->Pole(i);
        buf.Printf("(%f,%f,%f)", polepnt.X(), polepnt.Y(), polepnt.Z());
        rtn += buf;
    }

    rtn += "} ";
    rtn += EndedObject::ToString();
    return rtn;
}
