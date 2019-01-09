// HPoint.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "HPoint.h"
#include "Gripper.h"
#include "DigitizeMode.h"
#include "Drawing.h"

static unsigned char cross16[32] = {0x80, 0x01, 0x40, 0x02, 0x20, 0x04, 0x10, 0x08, 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80, 0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01};
static unsigned char cross16_selected[32] = {0xc0, 0x03, 0xe0, 0x07, 0x70, 0x0e, 0x38, 0x1c, 0x1c, 0x38, 0x0e, 0x70, 0x07, 0xe0, 0x03, 0xc0, 0x03, 0xc0, 0x07, 0xe0, 0x0e, 0x70, 0x1c, 0x38, 0x38, 0x1c, 0x70, 0x0e, 0xe0, 0x07, 0xc0, 0x03};


HPoint::HPoint()
 : HeeksObj(ObjType)
{
    InitializeProperties();
    m_draw_unselected = true;
}

HPoint::HPoint(const gp_Pnt &p, const HeeksColor& col)
 : HeeksObj(ObjType)
{
    InitializeProperties();
	m_p = p;
	SetColor ( col );
	m_draw_unselected = true;
}

HPoint::HPoint(const HPoint &p)
 : HeeksObj(p)
{
    InitializeProperties();
    operator =(p);
}

HPoint::~HPoint(void)
{
}

void HPoint::InitializeProperties()
{
    m_p.Initialize(_("Location"), this, true);
}

const HPoint& HPoint::operator=(const HPoint &b)
{
    HeeksObj::operator =(b);
	m_draw_unselected = b.m_draw_unselected;
	return *this;
}

const wxBitmap &HPoint::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/point.png")));
	return *icon;
}

bool HPoint::IsDifferent(HeeksObj* o)
{
	HPoint* other = (HPoint*)o;
	if(m_p.Distance(other->m_p) > wxGetApp().m_geom_tol)
		return true;

	return HeeksObj::IsDifferent(o);
}

void HPoint::glCommands(bool select, bool marked, bool no_color)
{
	if(!no_color){
		wxGetApp().glColorEnsuringContrast(GetColor());
	}
	GLfloat save_depth_range[2];
	if(marked){
		glGetFloatv(GL_DEPTH_RANGE, save_depth_range);
		glDepthRange(0, 0);
	}
	else if(!m_draw_unselected)
	{
		glBegin(GL_POINTS);
		glVertex3d(m_p.X(), m_p.Y(), m_p.Z());
		glEnd();
		return;
	}

	glRasterPos3d(m_p.X(), m_p.Y(), m_p.Z());
	glBitmap(16, 16, 8, 8, 10.0, 0.0, marked ? cross16_selected : cross16);
	if(marked){
		glDepthRange(save_depth_range[0], save_depth_range[1]);
	}
}

void HPoint::GetBox(CBox &box)
{
	box.Insert(m_p.X(), m_p.Y(), m_p.Z());
}

HeeksObj *HPoint::MakeACopy(void)const
{
	return new HPoint(*this);
}

void HPoint::ModifyByMatrix(const double *m)
{
	gp_Trsf mat = make_matrix(m);
	m_p.Transform(mat);
}

void HPoint::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	if(just_for_endof)
	{
		list->push_back(GripData((EnumGripperType)0,m_p.X(),m_p.Y(),m_p.Z(),NULL));
	}
}

bool HPoint::GetStartPoint(double* pos)
{
    extract(m_p, pos);
    return true;
}

bool HPoint::GetEndPoint(double* pos)
{
    extract(m_p, pos);
    return true;
}

//static
HeeksObj* HPoint::ReadFromXMLElement(TiXmlElement* pElem)
{
	HPoint* new_object = new HPoint();
	new_object->ReadBaseXML(pElem);
	return new_object;
}

void HPoint::Draw(wxDC& dc)
{
	wxGetApp().PlotSetColor(GetColor());
	double s[3], e[3];
	double line_length = 1.5;
	extract(m_p, s);
	extract(m_p, e); e[0] -= line_length; e[1] -= line_length; wxGetApp().PlotLine(s, e);
	extract(m_p, e); e[0] += line_length; e[1] -= line_length; wxGetApp().PlotLine(s, e);
	extract(m_p, e); e[0] -= line_length; e[1] += line_length; wxGetApp().PlotLine(s, e);
	extract(m_p, e); e[0] += line_length; e[1] += line_length; wxGetApp().PlotLine(s, e);
}

HPoint* point_for_tool = NULL;

class FilletAtPoint : public Tool
{
public:
    void Run() {
        double rad = 2.0;
        HeeksConfig& config = wxGetApp().GetConfig();
        config.Read(_T("PointFilletRadius"), &rad);
        if(wxGetApp().InputLength(_("Enter Fillet Radius"), _("Radius"), rad))
        {
            CSketch* sketch = dynamic_cast<CSketch *>(point_for_tool->GetOwner()->GetOwner());
            sketch->FilletAtPoint(point_for_tool->m_p, rad);
            config.Write(_T("PointFilletRadius"), rad);
        }
    }
    const wxChar* GetTitle(){return _("Fillet At Point");}
    wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/fillet.png");}
};
static FilletAtPoint fillet_at_point;

void HPoint::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
    point_for_tool = this;
    if (GetOwner() != NULL && GetOwner()->GetOwner() != NULL) {
        CSketch* sketch = dynamic_cast<CSketch *>(GetOwner()->GetOwner());
        if (sketch != NULL) {
            t_list->push_back(&fillet_at_point);
        }
    }
}
