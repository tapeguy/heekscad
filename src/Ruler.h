// Ruler.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"
class HeeksConfig;

struct RulerMark{
	int pos; // distance from the 0 mark
	double length;
	double width;

	void glCommands(double units); // including text
};

class HRuler: public HeeksObj{
	//      ___________________________________________________________________
	//     |   |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||   |
	//     |   |    |    |    |    |    |    |    |    |    |    |    |    |   |
	//     |   |cm      1|        2|        3|        4|         |         |   |
	//     |                                                                   |
	//     |                                                                   |
	//      ___________________________________________________________________
	//

	int m_gl_list;

	void GetFourCorners(gp_Pnt *point);
	void CalculateMarks(std::list<RulerMark> &marks);
	double GetUnits();

public:
	gp_Trsf m_trsf; // position and orientation ( no scaling allowed )
	PropertyCheck m_use_view_units;
	PropertyChoice m_units_choice;
	double m_units; // used, if m_use_view_units == false; 1.0 for mm, 25.4 for inches
	PropertyLength m_width; // in mm
	PropertyLength m_length;// in mm
	PropertyLength m_empty_length; // space at each end in mm

	HRuler();
	HRuler(const HRuler& copy);
	HRuler& operator= ( const HRuler& copy );

	// HeeksObj's virtual functions
	void InitializeProperties();
	int GetType()const{return RulerType;}
	int GetMarkingFilter()const{return RulerMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void KillGLLists(void);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Ruler");}
	HeeksObj *MakeACopy(void)const;
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnPropertyEdit(Property& prop);
	void GetProperties(std::list<Property *> *list);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool GetScaleAboutMatrix(double *m);
	bool CanBeRemoved(){return false;}
	bool CanBeCopied(){return false;}

	void WriteToConfig(HeeksConfig& config);
	void ReadFromConfig(HeeksConfig& config);
};
