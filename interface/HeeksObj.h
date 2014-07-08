// HeeksObj.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Box.h"
#include "wx/dc.h"

#include <list>
#include "MutableObject.h"
#include "Property.h"
#include "ObjTypes.h"

class HeeksColor;
class Tool;
class MarkedObject;
class TiXmlNode;
class TiXmlElement;
class GripData;
class TopoDS_Shape;
class ObjectCanvas;

#ifndef MULTIPLE_OWNERS
//    #define MULTIPLE_OWNERS
#endif


#ifdef HEEKSCAD
#define GET_ICON(X,Y) x = (X); y = (Y); texture_number = wxGetApp().m_icon_texture_number
#else
#define GET_ICON(X,Y) x = (X); y = (Y); texture_number = theApp.m_icon_texture_number
#endif

class HeeksObj : public MutableObject {
protected:
#ifdef MULTIPLE_OWNERS
	std::list<HeeksObj*> m_owners;
	std::list<HeeksObj*>::iterator m_owners_it;
#else
	HeeksObj* m_owner;
#endif

public:
	bool m_skip_for_undo;
	unsigned int m_layer;
	bool m_preserving_id;
	unsigned int m_index;

	HeeksObj(void);
	HeeksObj(const HeeksObj& copy);

	// Properties
	PropertyString   m_type;
	PropertyString   m_title;
	PropertyInt      m_id;
	PropertyCheck    m_visible;
	PropertyColor    m_color;

	virtual ~HeeksObj();

	virtual const HeeksObj& operator=(const HeeksObj &ho);
	virtual void OnPropertySet(Property *prop);

	// virtual functions
	virtual void InitializeProperties();
	virtual int GetType()const{return UnknownType;}
	virtual int GetMarkingFilter()const{return UnknownMarkingFilter;}
	virtual int GetIDGroupType()const{return GetType();}
	virtual void glCommands(bool select, bool marked, bool no_color){};
	virtual void Draw(wxDC& dc){} // for printing
	virtual bool DrawAfterOthers(){return false;}
	virtual void GetBox(CBox &box){}
	virtual const wxChar* GetShortString(void)const{return NULL;}
	virtual const wxChar* GetTypeString(void)const{return _("Unknown");}
	const wxChar* GetShortStringOrTypeString(void)const{if(GetShortString())return GetShortString();return GetTypeString();}
	virtual bool CanEditString(void)const{return false;}
	virtual void OnEditString(const wxChar* str){}
	virtual void KillGLLists(void){};
	virtual HeeksObj *MakeACopy()const = 0;
	virtual HeeksObj *MakeACopyWithID();
	virtual void ReloadPointers(){}
	virtual void Disconnect(std::list<HeeksObj*>parents){}
	virtual void CopyFrom(const HeeksObj* object){}
	virtual void SetColor(const HeeksColor &col){ m_color = col; }
	virtual const HeeksColor& GetColor()const { return m_color; }
	virtual void ModifyByMatrix(const double *m){} // transform the object
	virtual bool GetStartPoint(double* pos){return false;}
	virtual bool GetEndPoint(double* pos){return false;}
	virtual bool GetCentrePoint(double* pos){return false;}
	virtual int GetCentrePoints(double* pos, double* pos2){if(GetCentrePoint(pos))return 1; return 0;}
	virtual bool GetMidPoint(double* pos){return false;}
	virtual bool GetScaleAboutMatrix(double *m);
	virtual ObjectCanvas* GetDialog(wxWindow* parent){return NULL;} // returns a window for editing the values of this object.
	virtual void GetOnEdit(bool(**callback)(HeeksObj*)){} // returns a function for doing edit with a dialog
	bool Edit(){ bool(*fn)(HeeksObj*) = NULL; GetOnEdit(&fn); if(fn)return (*fn)(this); else return false;}  // do edit with a dialog
	virtual const wxBitmap &GetIcon();
	virtual int Intersects(const HeeksObj *object, std::list< double > *rl)const{return 0;}
	virtual bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point){return false;}
	virtual bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point){return false;}
	virtual void GetTools(std::list<Tool*>* t_list, const wxPoint* p){}
	virtual void GetGripperPositionsTransformed(std::list<GripData> *list, bool just_for_endof);
	virtual void GetProperties(std::list<Property *> *list);
	virtual bool Stretch(const double *p, const double* shift, void* data){return false;} // return true, if undo stretch is done with Add and Delete
	virtual bool StretchTemporary(const double *p, const double* shift, void* data){Stretch(p, shift, data); return true;} // returns true, because Stretch was done.  If not done, then override and return false;
	virtual bool StretchTemporaryTransformed(const double *p, const double* shift, void* data);
	virtual void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction){}
	virtual bool CanAdd(HeeksObj* object){return false;}
	virtual bool CanAddTo(HeeksObj* owner){return true;}
	virtual bool DescendForUndo(){return true;}
	virtual bool GetSkipForUndo(){return m_skip_for_undo;}
	virtual void SetSkipForUndo(bool val){m_skip_for_undo = val;}
	virtual bool OneOfAKind(){return false;} // if true, then, instead of pasting, find the first object of the same type and copy object to it.
	virtual bool Add(HeeksObj* object, HeeksObj* prev_object);
	virtual bool IsDifferent(HeeksObj* other){return false;}
	virtual void Remove(HeeksObj* object){object->OnRemove();}
	virtual void OnAdd(){}
	virtual void OnRemove();
	virtual bool CanBeRemoved(){return true;}
	virtual bool CanBeDragged(){return true;}
	virtual bool CanBeCopied(){return true;}
	virtual HeeksObj* GetFirstChild(){return NULL;}
	virtual HeeksObj* GetNextChild(){return NULL;}
	virtual HeeksObj* GetAtIndex(int index){return NULL;}
	virtual int GetNumChildren(){return 0;}
	virtual std::list<HeeksObj *> GetChildren() const { std::list<HeeksObj *> empty; return(empty); }
	virtual bool AutoExpand(){return false;}
	virtual void GetTriangles(void(*callbackfunc)(const double* x, const double* n), double cusp, bool just_one_average_normal = true){} // [nine doubles, three doubles],  or [nine doubles, nine doubles] if just_one_average_normal = false
	virtual double Area()const{return 0.0;}
	virtual void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const{};
	virtual void WriteXML(TiXmlNode *root){}
#ifdef CONSTRAINT_TESTER
    //JT
	virtual void AuditHeeksObjTree4Constraints(HeeksObj * SketchPtr ,HeeksObj * mom,int level,bool ShowMsgInConsole,bool * constraintsAreOk);
    void HeeksObjOccurrenceInSketch(HeeksObj * Sketch,HeeksObj * Object, int * occurences,int FromLevel,bool ShowMsgInConsole);
    virtual void FindConstrainedObj(HeeksObj * Sketch,HeeksObj * Object,int * OccurenceOfObjectInSketch,int FromLevel,int level,bool ShowMsgInConsole);
#endif
	virtual void WriteBaseXML(TiXmlElement *element);
	virtual void ReadBaseXML(TiXmlElement* element);
	virtual void SetID(int id) { m_id = id; }
	virtual unsigned int GetID() { int id = m_id; return id; }
	virtual void OnSetID(int id);
	virtual bool UsesID() { return true; }
	virtual bool UsesColor() { return true; }
	bool OnVisibleLayer();
	HeeksObj* Owner();
	virtual void SetOwner(HeeksObj*);
#ifdef MULTIPLE_OWNERS
	virtual std::list<HeeksObj*> Owners();
	virtual bool HasOwner();
	virtual bool HasOwner(HeeksObj* obj);
	virtual void AddOwner(HeeksObj*);
	virtual void AddOwners(std::list<HeeksObj *> owners);
	virtual void RemoveOwners();
	virtual void RemoveOwner(HeeksObj*);
	virtual HeeksObj* GetFirstOwner();
	virtual HeeksObj* GetNextOwner();
#else
	virtual void RemoveOwner();
#endif
	virtual const TopoDS_Shape *GetShape() { return(NULL); }
	virtual bool IsTransient(){return false;}
	virtual bool IsList(){return false;}
	virtual HeeksObj *Find( const int type, const unsigned int id );
	virtual void SetIdPreservation(const bool flag) { m_preserving_id = flag; }
	virtual void ToString(char* buf, unsigned int* rlen, unsigned int len);
	virtual unsigned int GetIndex();
	virtual bool UsesCustomSubNames(){return false;}
protected:
	virtual void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
public:
	virtual void OnChangeViewUnits(const double units){}
};
