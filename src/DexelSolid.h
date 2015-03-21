#pragma once

#include "DexelGrid.h"
#include "StlSolid.h"

class CStlSolid;
class DexelTriangle;

class DexelSolid : public HeeksObj
{
private:

    double resolution;
    DexelGrid xy;
    DexelGrid xz;
    DexelGrid yz;

public:

    static const int ObjType = DexelSolidType;

    DexelSolid ( double resolution = 0.25 );

    // HeeksObj's virtual functions
    void glCommands(bool select, bool marked, bool no_color);
    HeeksObj *MakeACopy()const;

    void ResizeGrids ( int x, int y, int z );
    void FromStlSolid ( CStlSolid * solid );

    void CombineContoursToParallelSlice ( ContourPoints& slice,
                                          ContourList& solidA,
                                          ContourList& hollowA,
                                          ContourList& solidB,
                                          ContourList& hollowB );

    std::list<CStlTri> VolumeTile ( std::vector<gp_Pnt>& xyContours,
                                    std::vector<gp_Pnt>& xzContours,
                                    std::vector<gp_Pnt>& yzContours );

protected:

    void BuildDexelGridXZAndYZ ( CBox& box, std::list<DexelTriangle>& tris );
    void BuildDexelGridXY ( CBox& box, std::list<DexelTriangle>& tris );
    static int IntersectRayTriangle ( gp_Lin& ray, DexelTriangle * tri, gp_XYZ& intersection );
    static int IntersectTrianglePlane ( gp_XYZ pa, gp_XYZ pb, gp_XYZ pc, double level, gp_XYZ& p1, gp_XYZ& p2 );
};
