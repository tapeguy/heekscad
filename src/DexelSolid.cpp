#include "DexelSolid.h"


class DexelTriangle {
public:

    gp_XYZ p0;
    gp_XYZ p1;
    gp_XYZ p2;
    gp_Vec u;
    gp_Vec v;
    gp_Vec normal;

    double uu;
    double uv;
    double vv;
    double d;

    DexelTriangle ( CStlTri stlTri )
    : p0(stlTri.x[0][0], stlTri.x[0][1], stlTri.x[0][2]),
      p1(stlTri.x[1][0], stlTri.x[1][1], stlTri.x[1][2]),
      p2(stlTri.x[2][0], stlTri.x[2][1], stlTri.x[2][2]),
      u(p0, p1),
      v(p0, p2)
    {
        normal = u;
        normal.Cross(v);

        // Pre-compute barycentric coordinates
        uu = u.Dot ( u );
        uv = u.Dot ( v );
        vv = v.Dot ( v );
        d = uv * uv - uu * vv;  // denominator of barycentric equa.
    }
};

DexelSolid::DexelSolid ( double resolution )
: HeeksObj(ObjType)
{
    this->resolution = resolution;
}

// Rehydrate triangles from triple-dexels.
//
void DexelSolid::glCommands ( bool select, bool marked, bool no_color )
{
    unsigned int i;

    ContourList::iterator It;
    ContourPoints::iterator It2;
    DexelContourPoint point;
    DexelContourPoint start;
    bool have_start;

    for ( i = 0; i < yz.GetRowSize ( ); i++ ) {
        ContourPoints xy_solid;
        ContourList xy_solid_contours;
        ContourList xy_hollow_contours;
        ContourList yx_solid_contours;
        ContourList yx_hollow_contours;

        // TODO:  Destruct lists!

        // Across the common Z axis
        yz.GroupDexelsIntoContours ( DexelGrid::ACROSS_ROW, i, xy_solid_contours, xy_hollow_contours );

            if ( i == 12 )
                for ( It = xy_solid_contours.begin(); It != xy_solid_contours.end(); It++ ) {
                    DexelContour * contour = *It;
                    if ( ! contour->IsEmpty ( ) ) {
                        DexelContourIterator It2 ( *contour );
                        have_start = false;

                        glBegin(GL_LINE_STRIP);
                        wxGetApp().glColorEnsuringContrast(HeeksColor(255, 0, 0));
                        point = It2.Next();
                        glVertex3d ( point.x, point.y * resolution, i * resolution );
                        while ( ! have_start || point.x != start.x || point.y != start.y ) {
                            if ( ! have_start ) {
                                start = point;
                                have_start = true;
                            }
                            point = It2.Next();
                            glVertex3d ( point.x, point.y * resolution, i * resolution );
                        }
                        glEnd();

                        It2.SetStart(point);
                        glPointSize(5.0f);
                        glBegin(GL_POINTS);
                        wxGetApp().glColorEnsuringContrast(HeeksColor(255, 0, 0));
                        point = It2.Next();
                        glVertex3d ( point.x, point.y * resolution, i * resolution );
                        while ( ! have_start || point.x != start.x || point.y != start.y ) {
                            if ( ! have_start ) {
                                start = point;
                                have_start = true;
                            }
                            point = It2.Next();
                            glVertex3d ( point.x, point.y * resolution, i * resolution );
                        }
                        glEnd();
                    }
                }

        xz.GroupDexelsIntoContours ( DexelGrid::ACROSS_ROW, i, yx_solid_contours, yx_hollow_contours );

            if ( i == 12 )
                for ( It = yx_solid_contours.begin(); It != yx_solid_contours.end(); It++ ) {
                    DexelContour * contour = *It;
                    if ( ! contour->IsEmpty ( ) ) {
                        DexelContourIterator It2 ( *contour );
                        have_start = false;

                        glBegin(GL_LINE_STRIP);
                        wxGetApp().glColorEnsuringContrast(HeeksColor(0, 0, 255));
                        point = It2.Next();
                        glVertex3d ( point.y * resolution, point.x, i * resolution );
                        while ( ! have_start || point.x != start.x || point.y != start.y ) {
                            if ( ! have_start ) {
                                start = point;
                                have_start = true;
                            }
                            point = It2.Next();
                            glVertex3d ( point.y * resolution, point.x, i * resolution );
                        }
                        glEnd();

                        It2.SetStart(point);
                        glPointSize(5.0f);
                        glBegin(GL_POINTS);
                        wxGetApp().glColorEnsuringContrast(HeeksColor(0, 0, 255));
                        point = It2.Next();
                        glVertex3d ( point.y * resolution, point.x, i * resolution );
                        while ( ! have_start || point.x != start.x || point.y != start.y ) {
                            if ( ! have_start ) {
                                start = point;
                                have_start = true;
                            }
                            point = It2.Next();
                            glVertex3d ( point.y * resolution, point.x, i * resolution );
                        }
                        glEnd();
                    }
                }

            this->CombineContoursToParallelSlice ( xy_solid, xy_solid_contours, xy_hollow_contours,
                                                             yx_solid_contours, yx_hollow_contours );

//            glBegin(GL_LINE_STRIP);
//            wxGetApp().glColorEnsuringContrast(HeeksColor(0, 0, 0));
//            glLineWidth(2);
//            for ( It2 = xy_solid.begin(); It2 != xy_solid.end(); It2++ ) {
//                point = *It2;
//                glVertex3d ( point.x, point.y, i * resolution );
//            }
//            if ( ! xy_solid.empty() ) {
//                point = xy_solid.front();
//                glVertex3d ( point.x, point.y, i * resolution );
//            }
//            glLineWidth(1);
//            glEnd();
    }

//    for ( i = 0; i < yz.GetColSize ( ); i++ ) {
//        ContourPoints xz_solid;
//        ContourList xz_solid_contours;
//        ContourList xz_hollow_contours;
//        ContourList zx_solid_contours;
//        ContourList zx_hollow_contours;
//
//        // Across the common Y axis
//        yz.GroupDexelsIntoContours ( DexelGrid::ACROSS_COLUMN, i, xz_solid_contours, xz_hollow_contours );
//        xy.GroupDexelsIntoContours ( DexelGrid::ACROSS_ROW, i, zx_solid_contours, zx_hollow_contours );
//
//        this->CombineContoursToParallelSlice ( xz_solid, xz_solid_contours, xz_hollow_contours,
//                                                         zx_solid_contours, zx_hollow_contours );
//    }
//
//    for ( i = 0; i < xz.GetColSize ( ); i++ ) {
//        ContourPoints yz_solid;
//        ContourList yz_solid_contours;
//        ContourList yz_hollow_contours;
//        ContourList zy_solid_contours;
//        ContourList zy_hollow_contours;
//
//        // Across the common X axis
//        xz.GroupDexelsIntoContours ( DexelGrid::ACROSS_COLUMN, i, yz_solid_contours, yz_hollow_contours );
//        xy.GroupDexelsIntoContours ( DexelGrid::ACROSS_COLUMN, i, zy_solid_contours, zy_hollow_contours );
//
//        this->CombineContoursToParallelSlice ( yz_solid, yz_solid_contours, yz_hollow_contours,
//                                                         zy_solid_contours, zy_hollow_contours );
//    }

//    std::list<CStlTri> tris = this->VolumeTile ( xy_solid, xz_solid, yz_solid );
}

HeeksObj * DexelSolid::MakeACopy ( ) const
{
    return NULL;
}

void DexelSolid::ResizeGrids ( int x, int y, int z )
{
    xy.ResizeTo(x, y);
    xz.ResizeTo(x, z);
    yz.ResizeTo(y, z);
}

void DexelSolid::FromStlSolid ( CStlSolid * solid )
{
    CBox box;
    solid->GetBox(box);
    this->ResizeGrids(box.Width() / resolution + 1,
                      box.Height() / resolution + 1,
                      box.Depth() / resolution + 1);

    std::list<DexelTriangle> tris;

    // Precompute vectors for all STL triangles.
    std::list<CStlTri>::iterator It;
    for (It = solid->m_list.begin(); It != solid->m_list.end(); It++)
    {
        CStlTri &t = *It;
        DexelTriangle tri ( t );
        tris.push_back ( tri );
    }

    this->BuildDexelGridXZAndYZ ( box, tris );
    this->BuildDexelGridXY ( box, tris );

    xy.SetColOffset ( box.MinX ( ) );
    xy.SetRowOffset ( box.MinY ( ) );
    xz.SetColOffset ( box.MinX ( ) );
    xz.SetRowOffset ( box.MinZ ( ) );
    yz.SetColOffset ( box.MinY ( ) );
    yz.SetRowOffset ( box.MinZ ( ) );

    xy.JoinUnevenPairs ( );
    xz.JoinUnevenPairs ( );
    yz.JoinUnevenPairs ( );
}

void DexelSolid::CombineContoursToParallelSlice ( ContourPoints& slice,
                                                  ContourList& solidA,
                                                  ContourList& hollowA,
                                                  ContourList& solidB,
                                                  ContourList& hollowB )
{
    DexelContour * contourA;
    DexelContour * contourB;
    ContourList::iterator It;

    for ( It = solidA.begin(); It != solidA.end(); It++ ) {
        contourA = *It;
        contourB = contourA->FindCorrespondingContour ( solidB );
        contourA->TravelAndCombine ( slice, contourB, this->resolution );
    }
}

std::list<CStlTri> DexelSolid::VolumeTile ( std::vector<gp_Pnt>& xyContours,
                                            std::vector<gp_Pnt>& xzContours,
                                            std::vector<gp_Pnt>& yzContours )
{
}

void DexelSolid::BuildDexelGridXZAndYZ ( CBox& box, std::list<DexelTriangle>& tris )
{
    int i, j;

    double min_value, max_value;
    double plane_scan = box.MinZ();  // take slices across the object in orth. plane
    gp_Lin rayx;
    gp_Vec rayx_incr;
    rayx.SetDirection(gp_Dir (1, 0, 0));  // find dexels in the x direction
    rayx_incr.SetY(resolution);           // scan across the slice

    gp_Lin rayy;
    gp_Vec rayy_incr;
    rayy.SetDirection(gp_Dir (0, 1, 0));  // find dexels in the Y direction (plane normal)
    rayy_incr.SetX(resolution);           // scan across the slice the other direction

    std::list<DexelTriangle>::iterator It;
    std::list<DexelTriangle>::iterator It2;
    gp_XYZ intersection;

    for ( i = 0; i < this->xz.GetRowSize ( ); i++ )
    {
        std::list<DexelTriangle> in_plane;

        // Find all mesh triangles in the requested plane.
        for (It = tris.begin(); It != tris.end(); It++)
        {
            DexelTriangle &t = *It;

            min_value = min ( t.p0.Z(), t.p1.Z(), t.p2.Z() );
            max_value = max ( t.p0.Z(), t.p1.Z(), t.p2.Z() );

            if ( plane_scan >= min_value && plane_scan <= max_value ) {
                in_plane.push_back(t);
            }
        }

        // Scan the plane for intersecting points.
        if ( in_plane.size() > 0 )
        {
            rayx.SetLocation ( gp_XYZ ( box.MinX ( ), box.MinY ( ), plane_scan ) );
            for ( j = 0; j < this->yz.GetColSize ( ); j++ )
            {
                for (It2 = in_plane.begin(); It2 != in_plane.end(); It2++)
                {
                    DexelTriangle& t = *It2;
                    if ( DexelSolid::IntersectRayTriangle ( rayx, &t, intersection ) == 1 ) {
                        this->yz.InsertAt(intersection.X(), j, i);
                    }
                }
                rayx.Translate(rayx_incr);
            }

            rayy.SetLocation ( gp_XYZ ( box.MinX ( ), box.MinY ( ), plane_scan ) );
            for ( j = 0; j < this->xz.GetColSize ( ); j++ )
            {
                for (It2 = in_plane.begin(); It2 != in_plane.end(); It2++)
                {
                    DexelTriangle& t = *It2;
                    if ( DexelSolid::IntersectRayTriangle ( rayy, &t, intersection ) == 1 ) {
                        this->xz.InsertAt(intersection.Y(), j, i);
                    }
                }
                rayy.Translate(rayy_incr);
            }
        }

        plane_scan += this->resolution;
    }
}

void DexelSolid::BuildDexelGridXY ( CBox& box, std::list<DexelTriangle>& tris )
{
    int i, j;
    double min_value, max_value;
    double plane_scan;
    gp_Lin rayz;
    gp_Vec rayz_incr;

    std::list<DexelTriangle>::iterator It2;
    gp_XYZ intersection;

    rayz.SetDirection(gp_Dir (0, 0, 1));  // find dexels in the Z direction (XY plane normal)
    plane_scan = box.MinY();              // take slices across the object in either orth. plane
    rayz_incr.SetX(resolution);           // scan across the slice

    for ( i = 0; i < this->xy.GetRowSize ( ); i++ )
    {
        std::list<DexelTriangle> in_plane;

        // Find all mesh triangles in the requested plane.
        std::list<DexelTriangle>::iterator It;
        for (It = tris.begin(); It != tris.end(); It++)
        {
            DexelTriangle &t = *It;

            min_value = min ( t.p0.Y(), t.p1.Y(), t.p2.Y() );
            max_value = max ( t.p0.Y(), t.p1.Y(), t.p2.Y() );

            if ( plane_scan >= min_value && plane_scan <= max_value ) {
                in_plane.push_back(t);
            }
        }

        // Scan the plane for intersecting points.
        if ( in_plane.size() > 0 )
        {
            rayz.SetLocation ( gp_XYZ ( box.MinX ( ), plane_scan, box.MinZ ( ) ) );
            for ( j = 0; j < this->xy.GetColSize ( ); j++ )
            {
                for (It2 = in_plane.begin(); It2 != in_plane.end(); It2++)
                {
                    DexelTriangle& t = *It2;
                    if ( DexelSolid::IntersectRayTriangle ( rayz, &t, intersection ) == 1 ) {
                        this->xy.InsertAt(intersection.Z(), j, i);
                    }
                }
                rayz.Translate(rayz_incr);
            }
        }

        plane_scan += this->resolution;
    }
}

// find the 3D intersection of a ray with a triangle
// output:     0 =  disjoint (no intersect)
//             1 =  intersect in unique point
//             2 =  are in the same plane
/* static */
int DexelSolid::IntersectRayTriangle ( gp_Lin& ray, DexelTriangle * tri, gp_XYZ& intersection )
{
    gp_XYZ    rloc = ray.Location().Coord();
    gp_Vec    dir = ray.Direction();

    gp_Vec    w0, w;           // ray vectors
    double    r, a, b;         // params to calc ray-plane intersect

    w0 = rloc - tri->p0;
    a = -tri->normal.Dot(w0);
    b = tri->normal.Dot(dir);

    if (fabs(b) < 0.000000001) {    // ray is parallel to triangle plane
        if (a == 0)                 // ray lies in triangle plane
            return 2;
        else
            return 0;               // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                    // ray goes away from triangle
        return 0;                   // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

    dir = dir * r;
    intersection = rloc + dir.XYZ();      // intersect point of ray and plane

    // is I inside T?
    double d, uu, uv, vv, wu, wv;
    d = tri->d;
    uu = tri->uu;
    uv = tri->uv;
    vv = tri->vv;
    w = intersection - tri->p0;
    wu = w.Dot(tri->u);
    wv = w.Dot(tri->v);

    // get and test parametric coords
    double s, t;
    s = (uv * wv - vv * wu) / d;
    if (s < 0.0 || s > 1.0)         // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / d;
    if (t < 0.0 || (s + t) > 1.0)   // I is outside T
        return 0;

    return 1;                       // I is in T
}


//   Create a contour slice through a 3 vertex facet pa,pb,pc
//   The contour "level" is a horizontal plane perpendicular to the z axis,
//        ie: The equation of the contour plane Ax + By + Cz + D = 0
//                 has A = 0, B = 0, C = 1, D = -level
//   Return
//         0 if the contour plane doesn't cut the facet
//         2 if it does cut the facet
//        -1 for an unexpected occurrence
//   If a vertex touches the contour plane nothing need to be drawn!?
//
/* static */
int IntersectTrianglePlane ( gp_XYZ pa, gp_XYZ pb, gp_XYZ pc, double level,
                             gp_XYZ& p1, gp_XYZ& p2 )
{
    double sidea,sideb,sidec;

    /*
      Evaluate the equation of the plane for each vertex
        sidea = A * pa.x + B * pa.y + C * pa.z + D;
        sideb = A * pb.x + B * pb.y + C * pb.z + D;
        sidec = A * pc.x + B * pc.y + C * pc.z + D;
    */
    sidea = pa.Z() - level;
    sideb = pb.Z() - level;
    sidec = pc.Z() - level;

    /* Are all the vertices on one side */
    if (sidea >= 0 && sideb >= 0 && sidec >= 0)
        return(0);
    if (sidea <= 0 && sideb <= 0 && sidec <= 0)
        return(0);

    /* Is p0 the only point on a side by itself */
    if ((std::signbit(sidea) != std::signbit(sideb)) && (std::signbit(sidea) != std::signbit(sidec))) {
        p1.SetX ( pa.X() - sidea * (pc.X() - pa.X()) / (sidec - sidea) );
        p1.SetY ( pa.Y() - sidea * (pc.Y() - pa.Y()) / (sidec - sidea) );
        p1.SetZ ( pa.Z() - sidea * (pc.Z() - pa.Z()) / (sidec - sidea) );
        p2.SetX ( pa.X() - sidea * (pb.X() - pa.X()) / (sideb - sidea) );
        p2.SetY ( pa.Y() - sidea * (pb.Y() - pa.Y()) / (sideb - sidea) );
        p2.SetZ ( pa.Z() - sidea * (pb.Z() - pa.Z()) / (sideb - sidea) );
        return(2);
    }

    /* Is p1 the only point on a side by itself */
    if ((std::signbit(sideb) != std::signbit(sidea)) && (std::signbit(sideb) != std::signbit(sidec))) {
        p1.SetX ( pb.X() - sideb * (pc.X() - pb.X()) / (sidec - sideb) );
        p1.SetY ( pb.Y() - sideb * (pc.Y() - pb.Y()) / (sidec - sideb) );
        p1.SetZ ( pb.Z() - sideb * (pc.Z() - pb.Z()) / (sidec - sideb) );
        p2.SetX ( pb.X() - sideb * (pa.X() - pb.X()) / (sidea - sideb) );
        p2.SetY ( pb.Y() - sideb * (pa.Y() - pb.Y()) / (sidea - sideb) );
        p2.SetZ ( pb.Z() - sideb * (pa.Z() - pb.Z()) / (sidea - sideb) );
        return(2);
    }

    /* Is p2 the only point on a side by itself */
    if ((std::signbit(sidec) != std::signbit(sidea)) && (std::signbit(sidec) != std::signbit(sideb))) {
        p1.SetX ( pc.X() - sidec * (pa.X() - pc.X()) / (sidea - sidec) );
        p1.SetY ( pc.Y() - sidec * (pa.Y() - pc.Y()) / (sidea - sidec) );
        p1.SetZ ( pc.Z() - sidec * (pa.Z() - pc.Z()) / (sidea - sidec) );
        p2.SetX ( pc.X() - sidec * (pb.X() - pc.X()) / (sideb - sidec) );
        p2.SetY ( pc.Y() - sidec * (pb.Y() - pc.Y()) / (sideb - sidec) );
        p2.SetZ ( pc.Z() - sidec * (pb.Z() - pc.Z()) / (sideb - sidec) );
        return(2);
    }

    /* Shouldn't get here */
    return(-1);
}
