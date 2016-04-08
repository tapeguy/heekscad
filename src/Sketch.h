     // Sketch.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/ObjList.h"
#include "../interface/SketchOrder.h"

class CoordinateSystem;

class CSketch : public IdNamedObjList
{
private:

	bool IsClockwise()const{return GetArea()>0;}
	std::map<int, int> order_map_for_properties; // maps drop-down index to SketchOrderType
	static std::string m_sketch_order_str[MaxSketchOrderTypes];

public:

	static const int ObjType = SketchType;


    PropertyVertex m_center;
	PropertyInt m_num_children;
	PropertyChoice m_order_choice;
	PropertyInt m_order;
	PropertyCheck m_solidify;
	bool m_draw_with_transform;
	CoordinateSystem* m_coordinate_system;

	CSketch();
	CSketch(const CSketch& c);
	virtual ~CSketch();

	const CSketch& operator=(const CSketch& c);

	bool operator== ( const CSketch & rhs ) const;
	bool operator!= ( const CSketch & rhs ) const { return(! (*this == rhs)); }

	bool IsDifferent( HeeksObj *other ) { return(*this != (*(CSketch *)other)); }

	virtual bool UsesColor() { return true; }
	void InitializeProperties();
	int GetMarkingFilter()const{return SketchMarkingFilter;}
	const wxBitmap &GetIcon();
	void OnPropertySet(Property& prop);
	bool GetStartPoint(double* pos);
	bool GetCentrePoint(double* pos);
	bool GetEndPoint(double* pos);
	void GetProperties(std::list<Property *> *list);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	HeeksObj *MakeACopy(void)const;
	void CopyFrom(const HeeksObj* object){operator=(*((CSketch*)((ObjList*)object)));}
	bool Add(HeeksObj* object, HeeksObj* prev_object = NULL);
	void Remove(HeeksObj* object);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	void CalculateSketchOrder();
	SketchOrderType GetSketchOrder();
	bool ReOrderSketch(SketchOrderType new_order); // returns true if done
	void ReLinkSketch(double tolerance);
	void ReverseSketch();
	void LinesToPoints(double tolerance);
	void RemoveDuplicates();
	void ExtractSeparateSketches(std::list<HeeksObj*> &new_separate_sketches, const bool allow_individual_objects = false);
	int Intersects(const HeeksObj *object, std::list< double > *rl) const;
	HeeksObj *Parallel( const double distance );
	bool FilletAtPoint(const gp_Pnt& p, double rad);
	static void ReverseObject(HeeksObj* object);
	static void SetObjectStartPoint ( HeeksObj* object, double* new_point );
	static void SetObjectEndPoint ( HeeksObj* object, double* new_point );
	double GetArea()const;
	CSketch* SplineToBiarcs(double tolerance)const;
};

class CSketchRelinker{
	const std::list<HeeksObj*> &m_old_list;
	std::set<HeeksObj*> m_added_from_old_set;
	std::list<HeeksObj*>::const_iterator m_old_front;
	HeeksObj* m_new_back;
	HeeksObj* m_new_front;
	bool AddNext(double tolerance);
	bool TryAdd(HeeksObj* object, double tolerance);

public:
	std::list< std::list<HeeksObj*> > m_new_lists;

	CSketchRelinker(const std::list<HeeksObj*>& old_list)
	: m_old_list(old_list), m_new_back(NULL), m_new_front(NULL){}

	bool Do(double tolerance); // makes m_new_lists
};
