// LineArcDrawing.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Drawing.h"

enum EnumDrawingMode{
	LineDrawingMode,
	ArcDrawingMode,
	ILineDrawingMode,
	CircleDrawingMode,
	EllipseDrawingMode,
	SplineDrawingMode
};

enum EnumCircleDrawingMode{
	CentreAndPointCircleMode,
	ThreePointsCircleMode,
	TwoPointsCircleMode,
	CentreAndRadiusCircleMode // only one click needed (edit radius in the properties before clicking)
};

enum EnumSplineDrawingMode{
	CubicSplineMode,
	QuarticSplineMode,
	RationalSplineMode
};


class LineArcDrawing: public Drawing{
private:
	std::list<DigitizedPoint> spline_points;
	bool m_A_down; // is key A pressed
	HeeksObj* m_container;
	bool m_add_to_sketch;

	static wxCursor m_cursor_start;
	static wxCursor m_cursor_end;
	std::list<EnumDrawingMode> m_save_drawing_mode;
	EnumSplineDrawingMode spline_mode;

	// Drawing's virtual functions
	bool calculate_item(DigitizedPoint &end);
	void set_previous_direction();
	int number_of_steps();
	int step_to_go_to_after_last_step();
	bool is_an_add_level(int level);
	bool is_a_draw_level(int level);
	HeeksObj* GetOwnerForDrawingObjects();
	void AddPoint();

public:
	PropertyChoice drawing_mode;
	PropertyLength radius_for_circle;
	PropertyChoice circle_mode;
	PropertyString arc_mode;

	bool m_previous_direction_set;
	gp_Vec m_previous_direction;

	LineArcDrawing(void);
	virtual ~LineArcDrawing(void);

	// InputMode's virtual functions
    void InitializeProperties();
    void OnPropertyEdit(Property& prop);
    void GetProperties(std::list<Property *> *list);
	const wxChar* GetTitle();
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void set_cursor(void);
	void GetTools(std::list<Tool*> *f_list, const wxPoint *p);
	bool OnModeChange(void);

	// Drawing's virtual functions
	void set_draw_step_not_undoable(int s);
	void AddObjectsMade();
};

