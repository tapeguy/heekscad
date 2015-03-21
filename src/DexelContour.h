#pragma once

#include "Dexel.h"

class DexelContour;
class DexelContourIterator;

class DexelContourPoint
{
public:

    double x;
    double y;

    void Set ( double x, double y ) {
        this->x = x;
        this->y = y;
    }

    DexelContourPoint& operator= ( const DexelContourPoint& copy ) {
        this->Set ( copy.x, copy.y );
        return *this;
    }

    bool operator== (const DexelContourPoint& rhs) const {
        return ( this->x == rhs.x && this->y == rhs.y );
    }

    bool operator!= (const DexelContourPoint& rhs) const {
        return ( ! (*this == rhs) );
    }
};

typedef std::vector<DexelContourPoint> ContourPoints;
typedef std::vector<ContourPoints> ParallelContours;
typedef std::vector<DexelContour *> ContourList;


class DexelContour
{
private:

    std::vector<Dexel *> contour;
    double offset;

public:

    DexelContour ( unsigned int height );

    ~DexelContour ( );

    Dexel * GetDexelRow ( unsigned int index ) const;

    Dexel * RightmostOfOverlapBelow ( Dexel * dexel, unsigned int index ) const;

    Dexel * LeftmostOfOverlapAbove ( Dexel * dexel, unsigned int index ) const;

    Dexel * LeftmostOfOverlapBelow ( Dexel * dexel, unsigned int index ) const;

    Dexel * RightmostOfOverlapAbove ( Dexel * dexel, unsigned int index ) const;

    void CopyDexelPair ( Dexel * dexel, unsigned int index );

    ContourPoints FindPointsInBox ( double x1, double x2, unsigned int y1, unsigned int y2 );

    void RemovePoint ( double value, unsigned int index );

    void RemoveAmbiguousPoints ( );

    void TravelAndCombine ( ContourPoints& slice, DexelContour * other, double resolution );

    DexelContour * FindCorrespondingContour ( ContourList& contourList );

    void SpliceContours ( DexelContour& other );

    bool IsEmpty ( ) const;

    void DebugOut ( ) const;

    double GetOffset ( ) const
    {
        return offset;
    }

    void SetOffset ( double offset )
    {
        this->offset = offset;
    }

    friend class DexelContourIterator;

private:

    void InsertAt ( double value, unsigned int index );
};


class DexelContourBox
{
private:

    double minx, maxx, miny, maxy;

public:

    DexelContourBox ( const DexelContour& contour );

    double OverlapAmount ( const DexelContourBox& other );
};


class DexelContourIterator {
private:

    const DexelContour& dexelContour;
    bool done;
    Dexel * start;
    Dexel * cursor;
    unsigned int row;

public:

    DexelContourIterator ( const DexelContour& contour );

    void SetStart ( DexelContourPoint& point );

    void SetCursor ( DexelContourPoint& point );

    DexelContourPoint Prev ( );     // Clock-wise

    DexelContourPoint Next ( );     // Counter clock-wise

    bool IsDone ( ) const { return done; }
};
