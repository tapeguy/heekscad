// ObjTypes.h

#include "EnumFactory.h"

// NOTE: If adding to this enumeration, please also add the verbose description to the HeeksCADType()) routine
// (NAME, ASSIGN, SHORT_VALUE)
#define ENUM_OBJ_TYPE(XX) \
	XX(UnknownType, = 0, _("Unknown")) \
	XX(DocumentType,, _("Document")) \
	XX(GripperType,, _("Gripper")) \
	XX(PointType,, _("Point")) \
	XX(LineType,, _("Line")) \
	XX(ArcType,, _("Arc")) \
	XX(ILineType,, _("Infinite Line")) \
	XX(CircleType,, _("Circle")) \
	XX(SketchType,, _("Sketch")) \
	XX(AreaType,, _("Area")) \
	XX(VertexType,, _("Vertex")) \
	XX(EdgeType,, _("Edge")) \
	XX(FaceType,, _("Face")) \
    XX(VertexListType,, _("Vertices")) \
    XX(EdgeListType,, _("Edges")) \
    XX(FaceListType,, _("Faces")) \
	XX(LoopType,, _("Loop")) \
	XX(SolidType,, _("Solid")) \
	XX(StlSolidType,, _("STL Solid")) \
	XX(WireType,, _("Wire")) \
	XX(CoordinateSystemType,, _("Coordinate System")) \
	XX(TextType,, _("Text")) \
	XX(DimensionType,, _("Dimension")) \
	XX(RulerType,, _("Ruler")) \
	XX(EllipseType,, _("Ellipse")) \
	XX(SplineType,, _("Spline")) \
	XX(GroupType,, _("Group")) \
	XX(CorrelationToolType,, _("Correlation")) \
	XX(ConstraintType,, _("Constraint")) \
	XX(PadType,, _("Pad")) \
	XX(PartType,, _("Part")) \
	XX(PocketSolidType,, _("Pocket")) \
	XX(AngularDimensionType,, _("Angular Dimension")) \
	XX(OrientationModifierType,, _("Orientation Modifier")) \
	XX(HoleType,, _("Hole")) \
	XX(HolePositionsType,, _("Hole Positions")) \
	XX(GearType,, _("Gear")) \
	XX(ImageType,, _("Image")) \
	XX(XmlType,, _("XML")) \
	XX(DexelSolidType,, _("Dexel")) \
	XX(ObjectMaximumType,, _("invalid"))

DECLARE_ENUM(ObjType, ENUM_OBJ_TYPE)

#define ENUM_MARKING_FILTER_TYPE(XX) \
	XX(UnknownMarkingFilter, = -1, _("Unknown")) \
	XX(LineMarkingFilter, = 0, _("line")) \
	XX(ArcMarkingFilter,, _("arc")) \
	XX(ILineMarkingFilter,, _("infinite line")) \
	XX(CircleMarkingFilter,, _("circle")) \
	XX(PointMarkingFilter,, _("point")) \
	XX(SolidMarkingFilter,, _("solid")) \
	XX(StlSolidMarkingFilter,, _("stl solid")) \
	XX(WireMarkingFilter,, _("wire")) \
	XX(FaceMarkingFilter,, _("face")) \
	XX(EdgeMarkingFilter,, _("edge")) \
	XX(SketchMarkingFilter,, _("sketch")) \
	XX(ImageMarkingFilter,, _("image")) \
	XX(CoordinateSystemMarkingFilter,, _("coordinate system")) \
	XX(TextMarkingFilter,, _("text")) \
	XX(DimensionMarkingFilter,, _("dimension")) \
	XX(RulerMarkingFilter,, _("ruler")) \
	XX(LoopMarkingFilter,, _("loop")) \
	XX(VertexMarkingFilter,, _("vertex")) \
	XX(PadMarkingFilter,, _("pad")) \
	XX(PartMarkingFilter,, _("part")) \
	XX(PocketSolidMarkingFilter,, _("pocket")) \
	XX(GearMarkingFilter,, _("gear")) \
	XX(AreaMarkingFilter,, _("area")) \
	XX(MaximumMarkingFilter,, _("invalid"))

DECLARE_ENUM(MarkingFilter, ENUM_MARKING_FILTER_TYPE)

