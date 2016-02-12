// RegularShapesDrawing.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "RegularShapesDrawing.h"
#include "Sketch.h"
#include "HLine.h"
#include "HArc.h"
#include "HeeksFrame.h"
#include "InputModeCanvas.h"


RegularShapesDrawing::RegularShapesDrawing(void)
{
	InitializeProperties();
	m_drawing_mode = RectanglesRegularShapeMode;
	m_number_of_side_for_polygon = 6;
	m_rect_radius = 0.0;
	m_obround_radius = 2.0;
}

RegularShapesDrawing::~RegularShapesDrawing(void)
{
}

void RegularShapesDrawing::InitializeProperties()
{
	m_drawing_mode.Initialize(_("drawing mode"), this);
	m_drawing_mode.m_choices.push_back ( wxString ( _("draw rectangles") ) );
	m_drawing_mode.m_choices.push_back ( wxString ( _("draw polygons") ) );
	m_drawing_mode.m_choices.push_back ( wxString ( _("draw slots") ) );

	m_polygon_mode.Initialize(_("polygon mode"), this);
	m_polygon_mode.m_choices.push_back ( wxString ( _("exscribed circle") ) );
	m_polygon_mode.m_choices.push_back ( wxString ( _("inscribed circle") ) );

	m_number_of_side_for_polygon.Initialize(_("number of sides for polygon"), this);

	m_rect_radius.Initialize(_("rect radius"), this);
	m_obround_radius.Initialize(_("obround radius"), this);
}

void RegularShapesDrawing::ClearSketch()
{
    if(TempObject())((CSketch*)TempObject())->Clear();
}

bool RegularShapesDrawing::calculate_item(DigitizedPoint &end)
{
	if(end.m_type == DigitizeNoItemType)return false;

    if(TempObject() && TempObject()->GetType() != SketchType){
        ClearObjectsMade();
    }

    // make sure sketch exists
    if(TempObject()==NULL){
        AddToTempObjects(new CSketch);
    }

	gp_Trsf mat;
	gp_Dir xdir = gp_Dir(1, 0, 0).Transformed(mat);
	gp_Dir ydir = gp_Dir(0, 1, 0).Transformed(mat);
	gp_Dir zdir = gp_Dir(0, 0, 1).Transformed(mat);

	gp_Pnt p0 = GetStartPos().m_point;
	gp_Pnt p2 = end.m_point;

	double x = gp_Vec(p2.XYZ()) * gp_Vec(xdir.XYZ()) - gp_Vec(p0.XYZ()) * gp_Vec(xdir.XYZ());
	double y = gp_Vec(p2.XYZ()) * gp_Vec(ydir.XYZ()) - gp_Vec(p0.XYZ()) * gp_Vec(ydir.XYZ());

	gp_Pnt p1 = p0.XYZ() + xdir.XYZ() * x;
	gp_Pnt p3 = p0.XYZ() + ydir.XYZ() * y;

	// swap left and right, if user dragged to the left
	if(x < 0){
		gp_Pnt t = p0;
		p0 = p1;
		p1 = t;
		t = p3;
		p3 = p2;
		p2 = t;
	}

	// swap top and bottom, if user dragged upward
	if(y < 0){
		gp_Pnt t = p0;
		p0 = p3;
		p3 = t;
		t = p1;
		p1 = p2;
		p2 = t;
	}

	// add ( or modify ) lines and arcs
	switch(m_drawing_mode)
	{
	case RectanglesRegularShapeMode:
		CalculateRectangle(x, y, p0, p1, p2, p3, xdir, ydir, zdir);
		break;
	case PolygonsRegularShapeMode:
		CalculatePolygon(GetStartPos().m_point, end.m_point, zdir);
		break;
	case ObroundRegularShapeMode:
		CalculateObround(GetStartPos().m_point, end.m_point, xdir, zdir);
		break;
	}

	return true;
}

void RegularShapesDrawing::CalculateRectangle(double x, double y, const gp_Pnt& p0, const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3, const gp_Dir& xdir, const gp_Dir& ydir, const gp_Dir& zdir)
{
	bool radii_wanted = false;
	bool x_lines_disappear = false;
	bool y_lines_disappear = false;

	if(m_rect_radius > 0.0000000001)
	{
		if(fabs(x) - m_rect_radius*2 > -0.0000000001 && fabs(y) - m_rect_radius*2 > -0.0000000001)
			radii_wanted = true;
		if(fabs(fabs(x) - m_rect_radius*2) < 0.0000000001)
			x_lines_disappear = true;
		if(fabs(fabs(y) - m_rect_radius*2) < 0.0000000001)
			y_lines_disappear = true;
	}

	int good_num = 4;

	if(radii_wanted)
	{
		if(x_lines_disappear && y_lines_disappear)good_num = 2;
		else if(x_lines_disappear || y_lines_disappear)good_num = 4;
		else good_num = 8;
	}

    if(TempObject()->GetNumChildren() != good_num)ClearSketch();
    // check first item
    else if(TempObject()->GetFirstChild()->GetType() != (radii_wanted ? ArcType:LineType))
        ClearSketch();

	if(radii_wanted)
	{
		if(x_lines_disappear && y_lines_disappear)
		{
            // make two arcs, for a circle
            HArc* arcs[2];
            if(TempObject()->GetNumChildren() > 0)
            {
                HeeksObj* object = TempObject()->GetFirstChild();
                for(int i = 0; i<2; i++)
                {
                    arcs[i] = (HArc*)object;
                    object = TempObject()->GetNextChild();
                }
            }
			else
			{
				for(int i = 0; i<2; i++)
				{
					arcs[i] = new HArc(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Circ(), wxGetApp().CurrentColor());
					TempObject()->Add(arcs[i]);
				}
			}
			arcs[0]->A->m_p = p0.XYZ() + xdir.XYZ() * m_rect_radius;
			arcs[0]->B->m_p = p3.XYZ() + xdir.XYZ() * m_rect_radius;
			arcs[0]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p0.XYZ() + xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
			arcs[1]->A->m_p = arcs[0]->B->m_p;
			arcs[1]->B->m_p = arcs[0]->A->m_p;
			arcs[1]->SetCircle(arcs[0]->GetCircle());
		}
		else if(x_lines_disappear || y_lines_disappear)
		{
			// arc-line-arc-line
			HArc* arcs[2];
			HLine* lines[2];
			if(TempObject()->GetNumChildren() > 0)
			{
				HeeksObj* object = TempObject()->GetFirstChild();
				for(int i = 0; i<2; i++)
				{
					arcs[i] = (HArc*)object;
					object = TempObject()->GetNextChild();
					lines[i] = (HLine*)object;
					object = TempObject()->GetNextChild();
				}
			}
			else
			{
				for(int i = 0; i<2; i++)
				{
					arcs[i] = new HArc(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Circ(), wxGetApp().CurrentColor());
					TempObject()->Add(arcs[i]);
					lines[i] = new HLine(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), wxGetApp().CurrentColor());
					TempObject()->Add(lines[i]);
				}
			}

			if(x_lines_disappear){
				arcs[0]->A->m_p = p2.XYZ() - ydir.XYZ() * m_rect_radius;
				arcs[0]->B->m_p = p3.XYZ() - ydir.XYZ() * m_rect_radius;
				arcs[0]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p3.XYZ() + xdir.XYZ() * m_rect_radius - ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
				lines[0]->A->m_p = arcs[0]->B->m_p;
				lines[0]->B->m_p = p0.XYZ() + ydir.XYZ() * m_rect_radius;
				arcs[1]->A->m_p = lines[0]->B->m_p;
				arcs[1]->B->m_p = p1.XYZ() + ydir.XYZ() * m_rect_radius;
				arcs[1]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p0.XYZ() + xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
				lines[1]->A->m_p = arcs[1]->B->m_p;
				lines[1]->B->m_p = arcs[0]->A->m_p;
			}
			else{
				arcs[0]->A->m_p = p1.XYZ() - xdir.XYZ() * m_rect_radius;
				arcs[0]->B->m_p = p2.XYZ() - xdir.XYZ() * m_rect_radius;
				arcs[0]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p1.XYZ() - xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
				lines[0]->A->m_p = arcs[0]->B->m_p;
				lines[0]->B->m_p = p3.XYZ() + xdir.XYZ() * m_rect_radius;
				arcs[1]->A->m_p = lines[0]->B->m_p;
				arcs[1]->B->m_p = p0.XYZ() + xdir.XYZ() * m_rect_radius;
				arcs[1]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p0.XYZ() + xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
				lines[1]->A->m_p = arcs[1]->B->m_p;
				lines[1]->B->m_p = arcs[0]->A->m_p;
			}
		}
		else{
			// arc-line-arc-line-arc-line-arc-line
			HLine* lines[4];
			HArc* arcs[4];
			if(TempObject()->GetNumChildren() > 0)
			{
				HeeksObj* object = TempObject()->GetFirstChild();
				for(int i = 0; i<4; i++)
				{
					arcs[i] = (HArc*)object;
					object = TempObject()->GetNextChild();
					lines[i] = (HLine*)object;
					object = TempObject()->GetNextChild();
				}
			}
			else
			{
				for(int i = 0; i<4; i++)
				{
					arcs[i] = new HArc(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Circ(), wxGetApp().CurrentColor());
					TempObject()->Add(arcs[i]);
					lines[i] = new HLine(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), wxGetApp().CurrentColor());
					TempObject()->Add(lines[i]);
				}
			}

			arcs[0]->A->m_p = p1.XYZ() - xdir.XYZ() * m_rect_radius;
			arcs[0]->B->m_p = p1.XYZ() + ydir.XYZ() * m_rect_radius;
			arcs[0]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p1.XYZ() - xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
			lines[0]->A->m_p = arcs[0]->B->m_p;
			lines[0]->B->m_p = p2.XYZ() - ydir.XYZ() * m_rect_radius;
			arcs[1]->A->m_p = lines[0]->B->m_p;
			arcs[1]->B->m_p = p2.XYZ() - xdir.XYZ() * m_rect_radius;
			arcs[1]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p2.XYZ() - xdir.XYZ() * m_rect_radius - ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
			lines[1]->A->m_p = arcs[1]->B->m_p;
			lines[1]->B->m_p = p3.XYZ() + xdir.XYZ() * m_rect_radius;
			arcs[2]->A->m_p = lines[1]->B->m_p;
			arcs[2]->B->m_p = p3.XYZ() - ydir.XYZ() * m_rect_radius;
			arcs[2]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p3.XYZ() + xdir.XYZ() * m_rect_radius - ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
			lines[2]->A->m_p = arcs[2]->B->m_p;
			lines[2]->B->m_p = p0.XYZ() + ydir.XYZ() * m_rect_radius;
			arcs[3]->A->m_p = lines[2]->B->m_p;
			arcs[3]->B->m_p = p0.XYZ() + xdir.XYZ() * m_rect_radius;
			arcs[3]->SetCircle(gp_Circ(gp_Ax2(gp_Pnt(p0.XYZ() + xdir.XYZ() * m_rect_radius + ydir.XYZ() * m_rect_radius), zdir), m_rect_radius));
			lines[3]->A->m_p = arcs[3]->B->m_p;
			lines[3]->B->m_p = arcs[0]->A->m_p;
		}
	}
	else
	{
		// line-line-line-line
		HLine* lines[4];
		if(TempObject()->GetNumChildren() > 0)
		{
			HeeksObj* object = TempObject()->GetFirstChild();
			for(int i = 0; i<4; i++)
			{
				lines[i] = (HLine*)object;
				object = TempObject()->GetNextChild();
			}
		}
		else
		{
			for(int i = 0; i<4; i++)
			{
				lines[i] = new HLine(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), wxGetApp().CurrentColor());
				TempObject()->Add(lines[i]);
			}
		}

		lines[0]->A->m_p = p0;
		lines[0]->B->m_p = p1;
		lines[1]->A->m_p = p1;
		lines[1]->B->m_p = p2;
		lines[2]->A->m_p = p2;
		lines[2]->B->m_p = p3;
		lines[3]->A->m_p = p3;
		lines[3]->B->m_p = p0;
	}
}

void RegularShapesDrawing::CalculatePolygon(const gp_Pnt& p0, const gp_Pnt& p1, const gp_Dir& zdir)
{
	if(p0.IsEqual(p1, wxGetApp().m_geom_tol))return;

	if(TempObject()->GetNumChildren() != m_number_of_side_for_polygon)
		ClearSketch();
	HLine** lines = (HLine**)malloc(m_number_of_side_for_polygon * sizeof(HLine*));

	if(TempObject()->GetNumChildren() > 0)
	{
		HeeksObj* object = TempObject()->GetFirstChild();
		for(int i = 0; i<m_number_of_side_for_polygon; i++)
		{
			lines[i] = (HLine*)object;
			object = TempObject()->GetNextChild();
		}
	}
	else
	{
		for(int i = 0; i<m_number_of_side_for_polygon; i++)
		{
			lines[i] = new HLine(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), wxGetApp().CurrentColor());
			TempObject()->Add(lines[i]);
		}
	}

    double radius = p0.Distance(p1);
    double sideAngle=0;
    double angle0;
    double angle1;

    switch (m_polygon_mode)
    {
        case InscribedMode:
            //inscribed circle
            sideAngle =2.0 * M_PI / m_number_of_side_for_polygon;
            radius = radius/cos((sideAngle/2));
            for(int i = 0; i<m_number_of_side_for_polygon; i++)
            {
                gp_Dir xdir(make_vector(p0, p1));
                gp_Dir ydir = zdir ^ xdir;
                angle0 = (sideAngle * i)+(sideAngle/2);
                angle1 = (sideAngle * (i+1))+(sideAngle/2);
                lines[i]->A->m_p = p0.XYZ() + xdir.XYZ() * ( cos(angle0) * radius ) + ydir.XYZ() * ( sin(angle0) * radius );
                if(i == m_number_of_side_for_polygon - 1)lines[i]->B->m_p = lines[0]->A->m_p;
                lines[i]->B->m_p = p0.XYZ() + xdir.XYZ() * ( cos(angle1) * radius ) + ydir .XYZ()* ( sin(angle1) * radius );
            }
        break;
        case ExcribedMode:
            //excribed circle
            for(int i = 0; i<m_number_of_side_for_polygon; i++)
            {
                gp_Dir xdir(make_vector(p0, p1));
                gp_Dir ydir = zdir ^ xdir;
                angle0 = 2.0 * M_PI / m_number_of_side_for_polygon * i;
                angle1 = 2.0 * M_PI / m_number_of_side_for_polygon * (i+1);
                lines[i]->A->m_p = p0.XYZ() + xdir.XYZ() * ( cos(angle0) * radius ) + ydir.XYZ() * ( sin(angle0) * radius );
                if(i == m_number_of_side_for_polygon - 1)lines[i]->B->m_p = lines[0]->A->m_p;
                lines[i]->B->m_p = p0.XYZ() + xdir.XYZ() * ( cos(angle1) * radius ) + ydir .XYZ()* ( sin(angle1) * radius );
            }
        break;
    }
	free(lines);
}

void RegularShapesDrawing::CalculateObround(const gp_Pnt& p0, const gp_Pnt& p1, const gp_Dir& xdir, const gp_Dir& zdir)
{
	bool lines_disappear = false;

	if(m_obround_radius > 0.0000000001)
	{
		if(p0.IsEqual(p1, wxGetApp().m_geom_tol))lines_disappear = true;
	}
	else return;

	int good_num = 4;
	if(lines_disappear)good_num = 2;

	if(TempObject()->GetNumChildren() != good_num)ClearSketch();

	if(lines_disappear)
	{
		// make two arcs, for a circle
		HArc* arcs[2];
		if(TempObject()->GetNumChildren() > 0)
		{
			HeeksObj* object = TempObject()->GetFirstChild();
			for(int i = 0; i<2; i++)
			{
				arcs[i] = (HArc*)object;
				object = TempObject()->GetNextChild();
			}
		}
		else
		{
			for(int i = 0; i<2; i++)
			{
				arcs[i] = new HArc(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Circ(), wxGetApp().CurrentColor());
				TempObject()->Add(arcs[i]);
			}
		}
		arcs[0]->A->m_p = p0.XYZ() + xdir.XYZ() * m_obround_radius;
		arcs[0]->B->m_p = p0.XYZ() - xdir.XYZ() * m_obround_radius;
		arcs[0]->SetCircle(gp_Circ(gp_Ax2(p0, zdir), m_obround_radius));
        arcs[1] = arcs[0];

        // Swap start and end
		arcs[1]->A->m_p = arcs[0]->B->m_p;
		arcs[1]->B->m_p = arcs[0]->A->m_p;
	}
	else
	{
		// arc-line-arc-line
		HArc* arcs[2];
		HLine* lines[2];
		if(TempObject()->GetNumChildren() > 0)
		{
			HeeksObj* object = TempObject()->GetFirstChild();
			for(int i = 0; i<2; i++)
			{
				arcs[i] = (HArc*)object;
				object = TempObject()->GetNextChild();
				lines[i] = (HLine*)object;
				object = TempObject()->GetNextChild();
			}
		}
		else
		{
			for(int i = 0; i<2; i++)
			{
				arcs[i] = new HArc(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Circ(), wxGetApp().CurrentColor());
				TempObject()->Add(arcs[i]);
				lines[i] = new HLine(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), wxGetApp().CurrentColor());
				TempObject()->Add(lines[i]);
			}
		}

		gp_Dir along_dir(make_vector(p0, p1));
		gp_Dir right_dir = along_dir ^ zdir;

		arcs[0]->A->m_p = p1.XYZ() + right_dir.XYZ() * m_obround_radius;
		arcs[0]->B->m_p = p1.XYZ() - right_dir.XYZ() * m_obround_radius;
		arcs[0]->SetCircle(gp_Circ(gp_Ax2(p1, zdir), m_obround_radius));
		lines[0]->A->m_p = arcs[0]->B->m_p;
		lines[0]->B->m_p = p0.XYZ() - right_dir.XYZ() * m_obround_radius;
		arcs[1]->A->m_p = lines[0]->B->m_p;
		arcs[1]->B->m_p = p0.XYZ() + right_dir.XYZ() * m_obround_radius;
		arcs[1]->SetCircle(gp_Circ(gp_Ax2(p0, zdir), m_obround_radius));
		lines[1]->A->m_p = arcs[1]->B->m_p;
		lines[1]->B->m_p = arcs[0]->A->m_p;
	}
}

const wxChar* RegularShapesDrawing::GetTitle()
{
	switch(m_drawing_mode)
	{
	case RectanglesRegularShapeMode:
		return _("Rectangle drawing");

	case PolygonsRegularShapeMode:
		return _("Polygon drawing");

	case ObroundRegularShapeMode:
		return _("Obround drawing");

	default:
		return _("Regular shapes drawing");
	}
}

void RegularShapesDrawing::OnPropertyEdit(Property *prop)
{
	if (prop == &m_drawing_mode)
	{
		ClearSketch();
		wxGetApp().m_frame->RefreshInputCanvas();
	}
}

void RegularShapesDrawing::GetProperties(std::list<Property *> *list)
{
	// RectanglesRegularShapeMode
	m_rect_radius.SetVisible(m_drawing_mode == RectanglesRegularShapeMode);

	// ObroundRegularShapeMode
	m_obround_radius.SetVisible(m_drawing_mode == ObroundRegularShapeMode);

	// PolygonsRegularShapeMode
	m_number_of_side_for_polygon.SetVisible(m_drawing_mode == PolygonsRegularShapeMode);
	m_polygon_mode.SetVisible(m_drawing_mode == PolygonsRegularShapeMode);

	Drawing::GetProperties(list);
}

void RegularShapesDrawing::GetTools(std::list<Tool*> *f_list, const wxPoint *p){
	Drawing::GetTools(f_list, p);
}
