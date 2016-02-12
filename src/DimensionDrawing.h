// DimensionDrawing.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Drawing.h"
#include "HDimension.h"

class DimensionDrawing: public Drawing {
private:
	PropertyChoice m_mode;

	// Drawing's virtual functions
	bool calculate_item(DigitizedPoint &end);
	int number_of_steps(){return 3;}
	int step_to_go_to_after_last_step(){return 0;}
	bool is_an_add_level(int level){return level == 2;}
	bool is_a_draw_level(int level){return level >= 1;}

public:
	DimensionDrawing(void);
	virtual ~DimensionDrawing(void);

	// InputMode's virtual functions
	const wxChar* GetTitle(){return _("Dimension drawing");}
	void GetTools(std::list<Tool*> *f_list, const wxPoint *p);

	// Drawing's virtual functions
	bool DragDoneWithXOR(){return false;}
};

extern DimensionDrawing dimension_drawing;
