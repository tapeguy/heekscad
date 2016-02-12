// Sketch.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "ConversionTools.h"
#include "Sketch.h"
#include "HLine.h"
#include "HArc.h"
#include "HSpline.h"
#include "HeeksFrame.h"
#include "ObjPropsCanvas.h"
#include "../interface/Tool.h"
#include "FaceTools.h"
#include "CoordinateSystem.h"
#include "Wire.h"
#include <wx/numdlg.h>

#include "DigitizeMode.h"
#include "Drawing.h"
#include "Face.h"

extern CHeeksCADInterface heekscad_interface;

std::string CSketch::m_sketch_order_str[MaxSketchOrderTypes] = {
	std::string("unknown"),
	std::string("empty"),
	std::string("open"),
	std::string("reverse"),
	std::string("bad"),
	std::string("re-order"),
	std::string("clockwise"),
	std::string("counter-clockwise"),
	std::string("multiple"),
	std::string("has circles"),
};

CSketch::CSketch()
 : IdNamedObjList(ObjType), m_order(SketchOrderTypeUnknown)
{
	InitializeProperties();
	m_solidify.SetValue(false);
	m_coordinate_system = NULL;
	m_draw_with_transform = true;
}

CSketch::CSketch(const CSketch& c)
 : IdNamedObjList(c)
{
	InitializeProperties();
    HeeksObj::operator=(c);      // my properties only
    m_coordinate_system = c.m_coordinate_system;
    m_draw_with_transform = c.m_draw_with_transform;
}

CSketch::~CSketch()
{
}

const CSketch& CSketch::operator=(const CSketch& c)
{
    if (this != &c)
    {
        IdNamedObjList::operator =(c);

        m_coordinate_system = c.m_coordinate_system;
        m_draw_with_transform = c.m_draw_with_transform;
    }

	return *this;
}

void CSketch::InitializeProperties()
{
    m_center.Initialize(_("center"), this);
    m_center.SetReadOnly(true);
    m_center.SetTransient(true);

	m_num_children.Initialize(_("Number of elements"), this);
	m_num_children.SetReadOnly(true);
	m_num_children.SetTransient(true);

	m_order_choice.Initialize(_("order"), this);
	m_order_choice.SetTransient(true);

	m_order.Initialize(_("order"), this);
	m_order.SetVisible(false);

	m_solidify.Initialize(_("solidify"), this);
}

const wxBitmap &CSketch::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/sketch.png")));
	return *icon;
}

class SetOrderUndoable : public Undoable{
        CSketch* m_object;
        SketchOrderType m_old_order;

public:
        SketchOrderType m_new_order;
        SetOrderUndoable(CSketch* object, SketchOrderType old_order):m_object(object),m_old_order(old_order){}
        void Run(bool redo){if(redo)m_object->m_order = m_new_order;}
        const wxChar* GetTitle(){return _("Reorder Object");}
        void RollBack(){m_object->m_order = m_old_order;}
};

static bool SketchOrderAvailable(SketchOrderType old_order, SketchOrderType new_order)
{
	// can we change from the older order type to the new order type?
	if(old_order == new_order)return true;

	switch(old_order)
	{
	case SketchOrderTypeOpen:
		switch(new_order)
		{
		case SketchOrderTypeReverse:
			return true;
		default:
			break;
		}
		break;

	case SketchOrderTypeBad:
		switch(new_order)
		{
		case SketchOrderTypeReOrder:
			return true;
		default:
			break;
		}
		break;

	case SketchOrderTypeCloseCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCCW:
			return true;
		default:
			break;
		}
		break;

	case SketchOrderTypeCloseCCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCW:
			return true;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return false;
}

void CSketch::OnPropertySet(Property& prop)
{
	if (prop == m_order_choice)
	{
		std::map<int, int>::iterator FindIt = order_map_for_properties.find(m_order_choice);
		if(FindIt != order_map_for_properties.end())
		{
			if(ReOrderSketch((SketchOrderType)FindIt->second))
			{
				wxGetApp().m_frame->RefreshProperties();
			}
		}
	}
	else
	{
	    IdNamedObjList::OnPropertySet(prop);
	}
}

bool CSketch::GetCentrePoint(double* pos)
{
    CBox box;
    this->GetBox(box);
    box.Centre(pos);
    return true;
}

void CSketch::GetProperties(std::list<Property *> *list)
{
    double center[3];
    GetCentrePoint(center);
    m_center = make_point(center);
	m_num_children = IdNamedObjList::GetNumChildren();
	SketchOrderType sketch_order = GetSketchOrder();
	order_map_for_properties.clear();
	m_order_choice.m_choices.clear();
	int j = 0;
	for(int i = 0; i< MaxSketchOrderTypes; i++)
	{
		if(SketchOrderAvailable(sketch_order, (SketchOrderType)i))
		{
			order_map_for_properties.insert(std::pair<int, int>(j, i));
			m_order_choice.m_choices.push_back(Ctt(m_sketch_order_str[i].c_str()));
			if (sketch_order == i)
			{
			    m_order_choice = j;
			}
			j++;
		}
	}

	IdNamedObjList::GetProperties(list);
}

static CSketch* sketch_for_tools = NULL;

class SplitSketch:public Tool{
public:
	void Run(){
		if(sketch_for_tools == NULL)return;
		std::list<HeeksObj*> new_sketches;
		sketch_for_tools->ExtractSeparateSketches(new_sketches, true);
        wxGetApp().DeleteUndoably(sketch_for_tools);
        wxGetApp().AddUndoably(new_sketches, sketch_for_tools->GetOwner());
	}
	const wxChar* GetTitle(){return _("Split Sketch");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/splitsketch.png");}
};

static SplitSketch split_sketch;

class ConvertSketchToFace: public Tool
{
public:
	void Run()
	{
		std::list<TopoDS_Shape> faces;
		if(ConvertSketchToFaceOrWire(sketch_for_tools, faces, true))
		{
		    wxGetApp().StartHistory();
			for(std::list<TopoDS_Shape>::iterator It2 = faces.begin(); It2 != faces.end(); It2++)
			{
				TopoDS_Shape& face = *It2;
				wxGetApp().AddUndoably(new CFace(TopoDS::Face(face)), NULL, NULL);
			}
			wxGetApp().EndHistory();
		}
	}
	const wxChar* GetTitle(){return _("Convert sketch to face");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/la2face.png");}
};

static ConvertSketchToFace convert_sketch_to_face;


class ConvertSketchToWire: public Tool
{
public:
	void Run()
	{
		std::list<TopoDS_Shape> wires;
		if(ConvertSketchToFaceOrWire(sketch_for_tools, wires, false))
		{
		    wxGetApp().StartHistory();
			for(std::list<TopoDS_Shape>::iterator It2 = wires.begin(); It2 != wires.end(); It2++)
			{
				TopoDS_Shape& wire = *It2;
				wxGetApp().AddUndoably(new CWire(TopoDS::Wire(wire), _T("Wire from sketch")), NULL, NULL);
			}
			wxGetApp().EndHistory();
		}
	}
	const wxChar* GetTitle(){return _("Convert sketch to wire");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/la2face.png");}
};

static ConvertSketchToWire convert_sketch_to_wire;


HeeksObj *CSketch::Parallel( const double distance )
{
    try {
            double deviation = heekscad_interface.GetTolerance();
            std::list<TopoDS_Shape> wires;
            if(ConvertSketchToFaceOrWire(this, wires, false))
            {
                HeeksObj *sketch = heekscad_interface.NewSketch();
                for(std::list<TopoDS_Shape>::iterator It2 = wires.begin(); It2 != wires.end(); It2++)
                {
                    TopoDS_Shape& wire = *It2;
                    BRepOffsetAPI_MakeOffset offset_wire(TopoDS::Wire(wire));
                    offset_wire.Perform(distance);
                    ConvertWireToSketch(TopoDS::Wire(offset_wire.Shape()), sketch, deviation);
                }
                return(sketch);
            }
        }
		catch (Standard_Failure) {
			Handle_Standard_Failure e = Standard_Failure::Caught();
			// wxMessageBox(wxString(_("Error making offset")) + _T(": ") + Ctt(e->GetMessageString()));
			return(NULL);
		}

		return(NULL);
}

class CopyParallel: public Tool
{
public:
	void Run()
	{
	    wxString message(_("Use negative for smaller and Positive for larger)"));
	    wxString prompt(_("Enter the distance"));
	    wxString caption(_("Distance"));

		double distance;
		if(wxGetApp().InputDouble(wxString(_("Use negative for smaller and Positive for larger)") ), _("Enter the distance"), distance))
		{
			HeeksObj *parallel_sketch = sketch_for_tools->Parallel( double(distance) );
			if (parallel_sketch != NULL)
			{
			    wxGetApp().StartHistory();
			    wxGetApp().AddUndoably(parallel_sketch, NULL, NULL);
				wxGetApp().EndHistory();
			}
			else
			{
				wxMessageBox(wxString(_("Error making offset")));
			}
		}
	}
	const wxChar* GetTitle(){return _("Copy Parallel");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/la2face.png");}
};

static CopyParallel copy_parallel;


class SketchArcsToLines: public Tool
{
public:
	void Run()
	{
		HeeksObj* new_object = SplitArcsIntoLittleLines(sketch_for_tools);
		wxGetApp().Remove(sketch_for_tools);
		sketch_for_tools = NULL;
		wxGetApp().AddUndoably(new_object, NULL, NULL);
	}

	const wxChar* GetTitle(){return _("Split arcs to little lines");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/splitarcs.png");}
};

static SketchArcsToLines sketch_arcs_to_lines;

class ClickMidpointOfSketch: public Tool
{
public:
	void Run()
	{
		CBox box;
		sketch_for_tools->GetBox(box);
		double centre[3];
		box.Centre(centre);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(gp_Pnt(centre[0], centre[1], centre[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}

	const wxChar* GetTitle(){return _("Click midpoint");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_sketch_midpoint.png");}
};

ClickMidpointOfSketch click_midpoint_of_sketch;


class ClickNorthernMidpointOfSketch: public Tool
{
public:
	void Run()
	{
		CBox box;
		sketch_for_tools->GetBox(box);
		double centre[3];
		box.Centre(centre);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(gp_Pnt(centre[0], box.MaxY(), centre[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}

	const wxChar* GetTitle(){return _("Click centre-top point");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_sketch_centre_top.png");}
};

ClickNorthernMidpointOfSketch click_northern_midpoint_of_sketch;

class ClickSouthernMidpointOfSketch: public Tool
{
public:
	void Run()
	{
		CBox box;
		sketch_for_tools->GetBox(box);
		double centre[3];
		box.Centre(centre);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(gp_Pnt(centre[0], box.MinY(), centre[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}

	const wxChar* GetTitle(){return _("Click centre-bottom point");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_sketch_centre_bottom.png");}
};

ClickSouthernMidpointOfSketch click_southern_midpoint_of_sketch;

class ClickEasternMidpointOfSketch: public Tool
{
public:
	void Run()
	{
		CBox box;
		sketch_for_tools->GetBox(box);
		double centre[3];
		box.Centre(centre);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(gp_Pnt(box.MaxX(), centre[1], centre[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}

	const wxChar* GetTitle(){return _("Click centre-right point");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_sketch_centre_right.png");}
};

ClickEasternMidpointOfSketch click_eastern_midpoint_of_sketch;

class ClickWesternMidpointOfSketch: public Tool
{
public:
	void Run()
	{
		CBox box;
		sketch_for_tools->GetBox(box);
		double centre[3];
		box.Centre(centre);

		wxGetApp().m_digitizing->digitized_point = DigitizedPoint(gp_Pnt(box.MinX(), centre[1], centre[2]), DigitizeInputType);
		Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
		if (pDrawingMode != NULL)
		{
			pDrawingMode->AddPoint();
		}
	}

	const wxChar* GetTitle(){return _("Click centre-left point");}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/click_sketch_centre_left.png");}
};

ClickWesternMidpointOfSketch click_western_midpoint_of_sketch;

class RemoveDuplicateLines: public Tool
{
public:
    void Run()
    {
        sketch_for_tools->RemoveDuplicates();
    }

    const wxChar* GetTitle(){return _("Remove duplicate lines");}
    wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/remove_duplicate_lines.png");}
};

RemoveDuplicateLines remove_duplicate_lines;

void CSketch::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	sketch_for_tools = this;

	if (GetNumChildren() > 1)
	{
        t_list->push_back(&split_sketch);
        t_list->push_back(&remove_duplicate_lines);
	}

	t_list->push_back(&convert_sketch_to_face);
	// t_list->push_back(&convert_sketch_to_wire);
	t_list->push_back(&sketch_arcs_to_lines);
	t_list->push_back(&copy_parallel);

	Drawing *pDrawingMode = dynamic_cast<Drawing *>(wxGetApp().input_mode_object);
	if (pDrawingMode != NULL)
	{
		// We're drawing something.  Allow these options.
		t_list->push_back(&click_midpoint_of_sketch);
		t_list->push_back(&click_northern_midpoint_of_sketch);
		t_list->push_back(&click_southern_midpoint_of_sketch);
		t_list->push_back(&click_eastern_midpoint_of_sketch);
		t_list->push_back(&click_western_midpoint_of_sketch);
	}
}

// static
void CSketch::ReverseObject(HeeksObj* object)
{
	// reverse object
	switch(object->GetType()){
case LineType:
	((HLine*)object)->Reverse();
	break;
case ArcType:
	((HArc*)object)->Reverse();
	break;
case SplineType:
	((HSpline*)object)->Reverse();
	break;
default:
	break;
	}
}

HeeksObj *CSketch::MakeACopy(void)const
{
	return new CSketch(*this);
}

// static member function
HeeksObj* CSketch::ReadFromXMLElement(TiXmlElement* pElem)
{
	CSketch* new_object = new CSketch;
	new_object->ReadBaseXML(pElem);
	new_object->ReloadPointers();
	return (ObjList*)new_object;
}

SketchOrderType CSketch::GetSketchOrder()
{
	if(m_order == SketchOrderTypeUnknown)
	    CalculateSketchOrder();
	int order = m_order;
	return (SketchOrderType)order;
}

void CSketch::CalculateSketchOrder()
{
	if(m_objects.size() == 0)
	{
		m_order = SketchOrderTypeEmpty;
		return;
	}

	HeeksObj* prev_object = NULL;
	HeeksObj* first_object = NULL;

	bool well_ordered = true;

	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;

		if(object->GetType() == CircleType)
		{
			m_order = SketchOrderHasCircles;
			return;
		}

		if(prev_object)
		{
			double prev_e[3], s[3];
			if(!prev_object->GetEndPoint(prev_e))
			{
			    well_ordered = false;
			    break;
			}
			if(!object->GetStartPoint(s))
			{
			    well_ordered = false;
			    break;
			}
			if(!(make_point(prev_e).IsEqual(make_point(s), wxGetApp().m_geom_tol)))
			{
			    well_ordered = false;
			    break;
			}
		}

		if(first_object == NULL)
		    first_object = object;
		prev_object = object;
	}

	if(well_ordered)
	{
		if(prev_object && first_object)
		{
			double e[3], s[3];
			if(prev_object->GetEndPoint(e))
			{
				if(first_object->GetStartPoint(s))
				{
					if(make_point(e).IsEqual(make_point(s), wxGetApp().m_geom_tol))
					{
						// closed
						if(IsClockwise())
						    m_order = SketchOrderTypeCloseCW;
						else
						    m_order = SketchOrderTypeCloseCCW;
						return;
					}
				}
			}
		}

		m_order = SketchOrderTypeOpen;
		return;
	}

	m_order = SketchOrderTypeBad; // although it might still be multiple, but will have to wait until ReOrderSketch is done.
}

bool CSketch::ReOrderSketch(SketchOrderType new_order)
{
	SketchOrderType old_order = GetSketchOrder();
	bool done = false;

	switch(old_order)
	{
	case SketchOrderTypeOpen:
		switch(new_order)
		{
		case SketchOrderTypeReverse:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;

	case SketchOrderTypeBad:
		switch(new_order)
		{
		case SketchOrderTypeBad:
			break;
		default:
			ReLinkSketch();
			done = true;
			break;
		}
		break;

	case SketchOrderTypeCloseCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCCW:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;

	case SketchOrderTypeCloseCCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCW:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return done;
}

void CSketch::ReLinkSketch()
{
	CSketchRelinker relinker(m_objects);
	relinker.Do();

	std::list<HeeksObj*> new_list;

	for(std::list< std::list<HeeksObj*> >::iterator It = relinker.m_new_lists.begin(); It != relinker.m_new_lists.end(); It++)
	{
		for(std::list<HeeksObj*>::iterator It2 = It->begin(); It2 != It->end(); It2++)
		{
		    new_list.push_back(*It2);
		}
	}

	wxGetApp().DoUndoable(new ReorderTool(this, new_list));

	if(relinker.m_new_lists.size() > 1)
	{
		m_order = SketchOrderTypeMultipleCurves;
	}
	else
	{
		CalculateSketchOrder();
	}
}

void CSketch::ReverseSketch()
{
	if(m_objects.size() == 0)
	    return;

	std::list<HeeksObj*> new_list;
	std::list<HeeksObj*> old_list = m_objects;

	for(std::list<HeeksObj*>::iterator It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		HeeksObj* copy = object->MakeACopyWithSameID();
		ReverseObject(copy);
		new_list.push_front(copy);
	}

	Clear();
	for(std::list<HeeksObj*>::iterator It = new_list.begin(); It != new_list.end(); It++)
	{
		HeeksObj* object = *It;
		Add(object);
	}
}

void CSketch::RemoveDuplicates()
{
    std::list<HeeksObj*> new_list;
    std::list<HeeksObj*> old_list = m_objects;
    bool duplicate;

    for(std::list<HeeksObj*>::iterator It = m_objects.begin(); It != m_objects.end() ;It++)
    {
        HeeksObj* object1 = *It;

        duplicate = false;
        for(std::list<HeeksObj*>::iterator It2 = new_list.begin(); It2 != new_list.end() ;It2++)
        {
            HeeksObj* object2 = *It2;
            if ( object1->GetType() == object2->GetType() &&
                 ! object1->IsDifferent(object2) )
            {
                // Duplicate found
                duplicate = true;
                break;
            }
        }
        if (duplicate == false)
        {
            HeeksObj* object = *It;
            HeeksObj* copy = object->MakeACopyWithSameID();
            new_list.push_front(copy);
        }
    }

    Clear();
    for(std::list<HeeksObj*>::iterator It = new_list.begin(); It != new_list.end(); It++)
    {
        HeeksObj* object = *It;
        Add(object);
    }
}

void CSketch::ExtractSeparateSketches(std::list<HeeksObj*> &new_separate_sketches, const bool allow_individual_objects /* = false */ )
{
	CSketch* re_ordered_sketch = NULL;
	CSketch* sketch = this;

	if(sketch->GetSketchOrder() == SketchOrderHasCircles)
	{
	    if (re_ordered_sketch == NULL)
	        re_ordered_sketch = (CSketch*)sketch->MakeACopyWithSameID();
		std::list<HeeksObj*>::iterator It;
		for(HeeksObj* child = re_ordered_sketch->GetFirstChild(); child != NULL; child = re_ordered_sketch->GetNextChild())
		{
            if (child->GetType() == CircleType)
            {
                HeeksObj * circle = child;
                CSketch* new_object = new CSketch();
                new_object->SetColor(GetColor());
                new_object->Add(circle->MakeACopyWithSameID());
                new_separate_sketches.push_back(new_object);

                // ObjList correctly calls erase() to remove while iterating
                re_ordered_sketch->Remove(circle);
            }
		}
		sketch = re_ordered_sketch;
	}

	if(sketch->GetSketchOrder() == SketchOrderTypeBad)
	{
	    // Duplicate and reorder the sketch to see if it's possible to make separate connected sketches.
	    if (re_ordered_sketch == NULL)
	        re_ordered_sketch = (CSketch*)sketch->MakeACopyWithSameID();
		re_ordered_sketch->ReOrderSketch(SketchOrderTypeReOrder);
		sketch = re_ordered_sketch;
	}
	if(sketch->GetSketchOrder() == SketchOrderTypeMultipleCurves)
	{
        // Make separate connected sketches from the child elements.
		CSketchRelinker relinker(sketch->m_objects);

		relinker.Do();

		for(std::list< std::list<HeeksObj*> >::iterator It = relinker.m_new_lists.begin(); It != relinker.m_new_lists.end(); It++)
		{
			std::list<HeeksObj*>& list = *It;
			CSketch* new_object = new CSketch();
			new_object->SetColor(GetColor());
			for(std::list<HeeksObj*>::iterator It2 = list.begin(); It2 != list.end(); It2++)
			{
				HeeksObj* object = *It2;
				new_object->Add(object->MakeACopyWithSameID(), NULL);
			}
			new_separate_sketches.push_back(new_object);
		}
	}
	else if(sketch->GetSketchOrder() == SketchOrderTypeOpen ||
	        sketch->GetSketchOrder() == SketchOrderTypeCloseCW ||
	        sketch->GetSketchOrder() == SketchOrderTypeCloseCCW)
    {
	    new_separate_sketches.push_back(sketch->MakeACopyWithSameID());
    }
	else
	{
        // The sketch does not seem to relink into separate connected shapes.  Just export
        // all the sketch's children as separate objects instead.
		if (allow_individual_objects)
		{
			for (HeeksObj *child = sketch->GetFirstChild(); child != NULL; child = sketch->GetNextChild())
			{
				new_separate_sketches.push_back( child->MakeACopyWithSameID() );
			}
		}
	}

	if(re_ordered_sketch)
	    delete re_ordered_sketch;
}

double CSketch::GetArea()const
{
	double area = 0.0;

	for(std::list<HeeksObj*>::const_iterator It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		switch(object->GetType())
		{
		case ArcType:
			{
				double angle = ((HArc*)object)->IncludedAngle();
				double radius = ((HArc*)object)->Radius();
				double p0x = ((HArc*)object)->A->m_p.X();
				double p0y = ((HArc*)object)->A->m_p.Y();
				double p1x = ((HArc*)object)->B->m_p.X();
				double p1y = ((HArc*)object)->B->m_p.Y();
				double pcx = ((HArc*)object)->C->m_p.X();
				double pcy = ((HArc*)object)->C->m_p.Y();
				area += ( 0.5 * ((pcx - p0x) * (pcy + p0y) - (pcx - p1x) * (pcy + p1y) - angle * radius * radius));
			}
			break;
		default:
			// treat all others as lines
			{
				double s[3], e[3];
				if(!object->GetStartPoint(s))break;
				if(!object->GetEndPoint(e))break;
				area += (0.5 * (e[0] - s[0]) * (s[1] + e[1]));
			}
			break;
		}
	}

	return area;
}

bool CSketch::Add(HeeksObj* object, HeeksObj* prev_object)
{
	m_order = SketchOrderTypeUnknown;
	return IdNamedObjList::Add(object, prev_object);
}

void CSketch::Remove(HeeksObj* object)
{
	m_order = SketchOrderTypeUnknown;
	IdNamedObjList::Remove(object);
}

bool CSketchRelinker::TryAdd(HeeksObj* object)
{
	// if the object is not already added
	if(m_added_from_old_set.find(object) == m_added_from_old_set.end())
	{
		double old_point[3];
		double new_point[3];
		m_new_back->GetEndPoint(old_point);

		// try the object, the right way round
		object->GetStartPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopyWithSameID();
			m_new_lists.back().push_back(new_object);
			m_new_back = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}

		// try the object, the wrong way round
		object->GetEndPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopyWithSameID();
			CSketch::ReverseObject(new_object);
			m_new_lists.back().push_back(new_object);
			m_new_back = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}

		// try at the start
		m_new_front->GetStartPoint(old_point);

        // try the object, the right way round
        object->GetEndPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopyWithSameID();
			m_new_lists.back().push_front(new_object);
			m_new_front = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}

		// try the object, the wrong way round
		object->GetStartPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopyWithSameID();
			CSketch::ReverseObject(new_object);
			m_new_lists.back().push_front(new_object);
			m_new_front = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}
	}

	return false;
}

bool CSketchRelinker::AddNext()
{
	// returns true, if another object was added to m_new_lists

	if(m_new_back)
	{
		bool added = false;

		// look through all of the old list, starting at m_old_front
		std::list<HeeksObj*>::const_iterator It = m_old_front;
		do{
			It++;
			if(It == m_old_list.end())
			    It = m_old_list.begin();
			HeeksObj* object = *It;

			added = TryAdd(object);

		}while(It != m_old_front && !added);

		if(added)
		    return true;

		// nothing fits the current new list

		m_new_back = NULL;
		m_new_front = NULL;

		if(m_old_list.size() > m_added_from_old_set.size())
		{
			// there are still some to add, find a unused object
			for(std::list<HeeksObj*>::const_iterator It = m_old_list.begin(); It != m_old_list.end(); It++)
			{
				HeeksObj* object = *It;
				if(m_added_from_old_set.find(object) == m_added_from_old_set.end())
				{
					HeeksObj* new_object = object->MakeACopyWithSameID();
					std::list<HeeksObj*> empty_list;
					m_new_lists.push_back(empty_list);
					m_new_lists.back().push_back(new_object);
					m_added_from_old_set.insert(object);
					m_old_front = It;
					m_new_back = new_object;
					m_new_front = new_object;
					return true;
				}
			}
		}
	}

	return false;
}

bool CSketchRelinker::Do()
{
	if(m_old_list.size() > 0)
	{
		HeeksObj* new_object = m_old_list.front()->MakeACopyWithSameID();
		std::list<HeeksObj*> empty_list;
		m_new_lists.push_back(empty_list);
		m_new_lists.back().push_back(new_object);
		m_added_from_old_set.insert(m_old_list.front());
		m_old_front = m_old_list.begin();
		m_new_back = new_object;
		m_new_front = new_object;

		while(AddNext()){}
	}

	return true;
}


/**
	The Intersects() method is included in the heeks CAD interface as well as being
	a virtual method in the HeeksObj base class.  Since this Sketch object is, itself,
	simply a list of HeeksObj objects, we should be able to simply aggregate the
	intersection of the specified HeeksObj with all of 'our' HeeksObj objects.
 */
int CSketch::Intersects(const HeeksObj *object, std::list< double > *rl) const
{
	int number_of_intersections = 0;

	for (std::list<HeeksObj *>::const_iterator l_itObject = m_objects.begin(); l_itObject != m_objects.end(); l_itObject++)
	{
		number_of_intersections += (*l_itObject)->Intersects( object, rl );
	} // End for

	return(number_of_intersections);
} // End Intersects() method

bool CSketch::operator==( const CSketch & rhs ) const
{
	if (m_order != rhs.m_order)
	    return(false);
	if (m_solidify != rhs.m_solidify)
	    return(false);
	if (m_draw_with_transform != rhs.m_draw_with_transform)
	    return(false);

	return(IdNamedObjList::operator==(rhs));
}

static bool FindClosestVertex(const gp_Pnt& p, const TopoDS_Face &face, TopoDS_Vertex &closest_vertex)
{
	// find closest vertex
	TopExp_Explorer ex1;
	double best_dist = -1;

	for(ex1.Init(face,TopAbs_VERTEX); ex1.More(); ex1.Next())
	{
		TopoDS_Vertex Vertex =TopoDS::Vertex(ex1.Current());
		gp_Pnt pos = BRep_Tool::Pnt(Vertex);
		double d = pos.Distance(p);
		if(best_dist < 0 || d < best_dist)
		{
			best_dist = d;
			closest_vertex = Vertex;
		}
	}

	return best_dist > -0.1;
}

#undef Status

bool CSketch::FilletAtPoint(const gp_Pnt& p, double rad)
{
	std::list<TopoDS_Shape> faces;
	bool fillet_done = false;

	if(ConvertSketchToFaceOrWire(this, faces, true))
	{
		for(std::list<TopoDS_Shape>::iterator It2 = faces.begin(); It2 != faces.end(); It2++)
		{
			TopoDS_Shape& shape = *It2;
			if(shape.ShapeType() != TopAbs_FACE)continue;
			const TopoDS_Face& face = TopoDS::Face(shape);
			BRepFilletAPI_MakeFillet2d fillet(face);
			TopoDS_Vertex Vertex;
			if(!FindClosestVertex(p, face, Vertex))continue;
			fillet.AddFillet(Vertex, rad);
			if(fillet.Status() != ChFi2d_IsDone)continue;
			const TopoDS_Shape& new_shape = fillet.Shape();
			if(new_shape.ShapeType() != TopAbs_FACE)continue;
			const TopoDS_Face& new_face = TopoDS::Face(new_shape);
			HeeksObj* new_object = new CSketch();
			if(ConvertFaceToSketch2(new_face, new_object, FaceToSketchTool::deviation))
			{
				*this = *((CSketch*)new_object);
				delete new_object;
				fillet_done = true;
			}
		}
	}

	return fillet_done;
}

CSketch* CSketch::SplineToBiarcs(double tolerance)const
{
	CSketch *new_sketch = new CSketch;

	for(std::list<HeeksObj*>::const_iterator It = m_objects.begin(); It != m_objects.end(); It++)
	{
		HeeksObj* span = *It;
		if(span->GetType() == SplineType)
		{
			std::list<HeeksObj*> new_spans;
			((HSpline*)span)->ToBiarcs(new_spans, tolerance);
			for(std::list<HeeksObj*>::iterator ItS = new_spans.begin(); ItS != new_spans.end(); ItS++)
			{
				new_sketch->Add(*ItS, NULL);
			}
		}
		else
		{
			new_sketch->Add(span->MakeACopy(), NULL);
		}
	}

	return new_sketch;
}
