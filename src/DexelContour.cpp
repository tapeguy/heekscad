#include "DexelContour.h"


DexelContour::DexelContour ( unsigned int height )
{
    contour.resize ( height );
    this->offset = 0.0;
}

DexelContour::~DexelContour ( )
{
    Dexel * dexel;
    int i;
    for ( i = 0; i < contour.size(); i++ ) {
        dexel = contour[i];
        if ( dexel ) {
            dexel->Destroy ( );
        }
    }
}

Dexel * DexelContour::GetDexelRow ( unsigned int index ) const
{
    return contour[index];
}

Dexel * DexelContour::RightmostOfOverlapBelow ( Dexel * dexel, unsigned int index ) const
{
    if ( index == 0 ) {
        return NULL;                            // At bottom
    }

    Dexel * overlap = NULL;
    if ( dexel->IsRight ( ) ) {
        dexel = dexel->prev;
    }
    Dexel * cursor = contour[index - 1];
    while ( cursor ) {
        if ( cursor->IsPartiallyOverlappedBy ( *dexel ) ) {
             overlap = cursor->next;
        }
        cursor = cursor->next->next;
    }
    return overlap;
}

Dexel * DexelContour::LeftmostOfOverlapAbove ( Dexel * dexel, unsigned int index ) const
{
    if ( index == contour.size() - 2 ) {
        return NULL;                            // At top
    }

    if ( dexel->IsRight ( ) ) {
        dexel = dexel->prev;
    }
    Dexel * cursor = contour[index + 1];
    while ( cursor ) {
        if ( cursor->IsPartiallyOverlappedBy ( *dexel ) ) {
             return cursor;
        }
        cursor = cursor->next->next;
    }
    return NULL;
}

Dexel * DexelContour::LeftmostOfOverlapBelow ( Dexel * dexel, unsigned int index ) const
{
    if ( index == 0 ) {
        return NULL;                            // At bottom
    }

    if ( dexel->IsRight ( ) ) {
        dexel = dexel->prev;
    }
    Dexel * cursor = contour[index - 1];
    while ( cursor ) {
        if ( cursor->IsPartiallyOverlappedBy ( *dexel ) ) {
             return cursor;
        }
        cursor = cursor->next->next;
    }
    return NULL;
}

Dexel * DexelContour::RightmostOfOverlapAbove ( Dexel * dexel, unsigned int index ) const
{
    if ( index == contour.size() - 2 ) {
        return NULL;                            // At top
    }

    Dexel * overlap = NULL;
    if ( dexel->IsRight ( ) ) {
        dexel = dexel->prev;
    }
    Dexel * cursor = contour[index + 1];
    while ( cursor ) {
        if ( cursor->IsPartiallyOverlappedBy ( *dexel ) ) {
             overlap = cursor->next;
        }
        cursor = cursor->next->next;
    }
    return overlap;
}

ContourPoints DexelContour::FindPointsInBox ( double x1, double x2, unsigned int y1, unsigned int y2 )
{
    ContourPoints rtn;
    Dexel * cursor;
    double xmin, xmax;
    unsigned int i, ymin, ymax;

    xmin = min ( x1, x2 );
    xmax = max ( x1, x2 );
    ymin = min ( y1, y2, contour.size() - 1 );
    ymax = max ( y1, y2, 0 );
    for ( i = ymin; i <= ymax; i++ ) {
        for ( cursor = contour[i]; cursor; cursor = cursor->next ) {
            if ( cursor->value >= xmin && cursor->value <= xmax ) {
                DexelContourPoint point;
                point.x = cursor->value;
                point.y = i;
                rtn.push_back ( point );
            }
        }
    }
    return rtn;
}

void DexelContour::RemovePoint ( double value, unsigned int index )
{
    Dexel * cursor;
    cursor = contour[index];
    while ( cursor ) {
        if ( cursor->value - value < 0.000000001 ) {

            if ( cursor->prev ) {
                cursor->prev->next = cursor->next;
            }
            if ( cursor->next ) {
                cursor->next->prev = cursor->prev;
            }
            if ( cursor == contour[index] ) {
                contour[index] = cursor->next;
            }
            delete cursor;
            break;
        }
        cursor = cursor->next;
    }
}

void DexelContour::RemoveAmbiguousPoints ( )
{
    Dexel * cursor;
    Dexel * overlap;

    unsigned int i;
    bool found = false;

    // handle double overlap at bottom of contour
    for ( i = 0; i < contour.size() - 1; i++ ) {
        cursor = contour[i];
        while ( cursor ) {
            found = true;
            overlap = LeftmostOfOverlapAbove ( cursor, i );
            if ( overlap && cursor->next->next && cursor->next->next->IsPartiallyOverlappedBy ( *overlap ) ) {
                contour[i] = cursor->next->next;
                cursor->next->next->prev = NULL;
                delete cursor->next;
                delete cursor;
                cursor = contour[i];
                // TODO: delete the shorter of the 2?
            }
            cursor = cursor->next->next;
        }
        if ( found ) {
            break;
        }
    }

    found = false;

    // handle double overlap at top of contour
    for ( i = contour.size() - 1; i > 0; i-- ) {
        cursor = contour[i];
        while ( cursor ) {
            found = true;
            overlap = LeftmostOfOverlapBelow ( cursor, i );
            if ( overlap && cursor->next->next && cursor->next->next->IsPartiallyOverlappedBy ( *overlap ) ) {
                contour[i] = cursor->next->next;
                cursor->next->next->prev = NULL;
                delete cursor->next;
                delete cursor;
                cursor = contour[i];
                // TODO: delete the shorter of the 2?
            }
            cursor = cursor->next->next;
        }
        if ( found ) {
            break;
        }
    }
}

void DexelContour::TravelAndCombine ( ContourPoints& slice, DexelContour * other, double resolution )
{
    DexelContourIterator It ( *this );
    DexelContourIterator It2 ( *other );

    bool haveStart = false;
    DexelContourPoint pointA1 = It.Next ( );
    ContourPoints A;
    ContourPoints B;
    ContourPoints pointB1list;
    ContourPoints pointB2list;
    DexelContourPoint pointB1;
    DexelContourPoint pointB2;
    DexelContourPoint tempPoint;
    double A1x, A2x, B1x, B2x;
    double miny, maxy;

    if ( ! other ) {
        while ( ! It.IsDone( ) ) {
            tempPoint.Set ( pointA1.x, pointA1.y * resolution );
            slice.push_back ( tempPoint );
            pointA1 = It.Next ( );
        }
        return;
    }

    DexelContourPoint pointA2 = It.Next ( );

    // Find a pair of corresponding start points.
    while ( ! It.IsDone() ) {

        A1x = (int) floor ( pointA1.x / resolution );
        A2x = (int) floor ( pointA2.x / resolution );

        if ( abs ( A1x - A2x ) > 0 ) {

            if ( A1x < A2x ) {
                A1x++;
            }
            else if ( A1x > A2x ) {
                A2x++;
            }

            if ( pointA2.y == pointA1.y ) {
                miny = ( pointA1.y - 1 ) * resolution;
                maxy = ( pointA1.y + 1 ) * resolution;

                // Must be one above or one below A1 on A1x ray
                // x in this contour corresponds to y in other contour
                pointB1list = other->FindPointsInBox ( miny, maxy, A1x, A1x );
                pointB2list = other->FindPointsInBox ( miny, maxy, A2x, A2x );

                if ( pointB1list.size ( ) == 1  ) {
                    haveStart = true;
                    pointB1 = pointB1list.front ( );
                }
                else if ( pointB1list.size ( ) == 2 && pointB2list.size ( ) == 1 ) {
                    haveStart = true;
                    pointB1 = pointB1list.front ( );
                    if ( pointB1.x >= pointA1.y && pointB2.x <= pointA1.y ) {
                        pointB1 = pointB1list.back ( );
                    }
                }
                else if ( pointB1list.size ( ) == 2 && pointB2list.size ( ) == 2 ) {
//                        haveStart = true;
                    // TODO:  Find both points with same directional sequence as A1 -> A2
                }
            }
            else {
                // Must be between A1 and A2 on A1x ray
                // again, x in this contour corresponds to y in other contour
                pointB1list = other->FindPointsInBox ( pointA1.y * resolution, pointA2.y * resolution, A1x, A1x );
                if ( pointB1list.size ( ) == 1 ) {
                    haveStart = true;
                    pointB1 = pointB1list.front ( );
                }
            }
            if ( haveStart ) {
                It.SetStart ( pointA1 );
                It2.SetStart ( pointB1 );
                break;
            }
        }
        pointA1 = pointA2;
        pointA2 = It.Next ( );
    }

    while ( ! It.IsDone( ) ) {
        tempPoint.Set ( pointA1.x, pointA1.y * resolution );
        A.push_back ( tempPoint );
        pointA1 = It.Next ( );
    }

    while ( ! It2.IsDone( ) ) {
        tempPoint.Set ( pointB1.y * resolution, pointB1.x );
        B.push_back ( tempPoint );
        pointB1 = It2.Prev ( );
    }

    int i;
    int r = 0;      // point index in contour A
    int s = 0;      // point index in contour B
    int t;
    int o = 0;          // index of the last associated point in contour B

    do {
        A1x = (int) floor ( A[r].x / resolution );
        A2x = (int) floor ( A[r+1].x / resolution );
        if ( A1x != A2x ) {

            if ( A1x < A2x ) {
                A1x++;
            }
            else if ( A1x > A2x ) {
                A2x++;
            }

            A1x *= resolution;
            A2x *= resolution;

            for ( t = s; t < B.size(); t++ ) {
                if ( A[r+1].y >= min ( B[t].y, B[t+1].y ) &&
                     A[r+1].y <= max ( B[t].y, B[t+1].y ) &&
                     ( B[t].x == A1x || B[t].x == A2x ) ) {
                    o = t;
                    break;
                }
            }
        }
        else if ( s < B.size() - 1 ){
            B1x = (int) floor ( B[s].x / resolution ) ;
            B2x = (int) floor ( B[s+1].x / resolution );

            if ( B1x < B2x ) {
                B1x++;
            }
            else if ( B1x > B2x ) {
                B2x++;
            }

            B1x *= resolution;
            B2x *= resolution;

            if ( A[r].y >= min ( B[s - 1].y, B[s].y ) &&
                 A[r].y <= max ( B[s - 1].y, B[s].y ) &&
                 ( B1x == A1x ) ) {

                for ( t = s; t != B.size(); t++ ) {
                    if ( A[r+1].y >= min ( B[t].y, B[t+1].y ) &&
                         A[r+1].y <= max ( B[t].y, B[t+1].y ) &&
                         ( B[t].x == A1x || B[t].x == A2x ) ) {
                        o = t;
                    }
                }
            }
            else {
                slice.push_back ( A[r] );
                r++;
                continue;
            }
        }
        // Add points Ar, Bs, ... Bo, Ar+1 to the end of slice
        slice.push_back ( A[r] );
        for ( i = s; i <= o; i++ ) {
            slice.push_back ( B[i] );
        }
        s = o + 1;
        r++;
    } while ( /* s < ( B.size() - 1 ) && */ r < ( A.size() - 1 ) );

    slice.push_back ( A[r] );
    for ( i = s; i < B.size(); i++ ) {
        slice.push_back ( B[i] );
    }

//    // Walk both contours and add points.
//    for ( ;It3 != A.end(); It3++ ) {
//
//        pointA1 = pointA2;
//        pointA2 = *It3;
//
//        tempPoint.Set ( pointA1.x, pointA1.y );
//        slice.push_back ( tempPoint );
//
//        if ( other && ( !It2.IsDone() || pointB1 != pointB2 ) ) {
//            A1x = (int) floor ( pointA1.x / resolution );
//            A2x = (int) floor ( pointA2.x / resolution );
//
//            if ( abs ( A1x - A2x ) > 0 ) {
//
//                if ( A1x < A2x ) {
//                    A1x++;
//                }
//                else if ( A1x > A2x ) {
//                    A2x++;
//                }
//
//                if ( pointA1.y == pointA2.y ) {
//                    // if A1 and A2 on same ray, B1 must be between one above or one below ray
//                    miny = pointA1.y - resolution;
//                    maxy = pointA1.y + resolution;
//                }
//                else {
//                    // if A1 and A2 on different rays, B1 must be between A1 and A2
//                    miny = min ( pointA1.y, pointA2.y );
//                    maxy = max ( pointA1.y, pointA2.y );
//                }
//
//                if ( pointB1.y == A1x && pointB1.x >= miny && pointB1.x <= maxy ) {
//                    do {
//                        tempPoint.Set ( pointB1.y, pointB1.x );   // flip X and y
//                        slice.push_back ( tempPoint );
//
//                        B1x = (int) floor ( pointB1.x / resolution );
//                        B2x = (int) floor ( pointB2.x / resolution );
//
//                        if ( B1x < B2x ) {
//                            B1x++;
//                        }
//                        else if ( B1x > B2x ) {
//                            B2x++;
//                        }
//
//                        if ( pointB1.y == pointB2.y ) {
//                            // if B1 and B2 on same ray, A1 must be between one above or one below ray
//                            miny = pointB1.y - resolution;
//                            maxy = pointB1.y + resolution;
//                        }
//                        else {
//                            // if B1 and B2 on different rays, A1 must be between B1 and B2
//                            miny = min ( pointB1.y, pointB2.y );
//                            maxy = max ( pointB1.y, pointB2.y );
//                        }
//
//                        if ( pointA2.y == B1x && pointA2.x >= miny && pointA2.x <= maxy ) {
//                            break;
//                        }
//                        pointA2list = this->FindPointsInBox ( miny, maxy, B1x, B2x );
//                        if ( pointA2list.size() > 0 ) {
//                            if ( pointA2list.size() == 1 ) {
//                                pointA2 = pointA2list.front ( );
//                                It.SetCursor ( pointA2 );
//                            }
//                            break;
//                        }
//
//                        if ( ! It2.IsDone() ) {
//                            pointB1 = pointB2;
//                        }
//                        pointB2 = It2.Prev ( );
//
//                    } while ( pointB1 != pointB2 );
//                }
//                else {
//                    pointB1list = other->FindPointsInBox ( miny, maxy, A1x, A2x );
//                    if ( pointB1list.size ( ) == 1 ) {
//                        pointB1 = pointB1list.front ( );
//                        tempPoint.Set ( pointB1.x, pointB1.y );
//                        slice.push_back ( tempPoint );
//
//                        It2.SetCursor ( pointB1 );
//                        pointB1 = It2.Prev ( );
//                        pointB2 = It2.Prev ( );
//                    }
//                }
//            }
//        }
//    }
}

DexelContour * DexelContour::FindCorrespondingContour ( ContourList& contourList )
{
    DexelContour * contour = NULL;
    DexelContour * otherContour;
    DexelContourBox myBox ( *this );
    double overlap, potential;
    ContourList::iterator It;

    for ( It = contourList.begin(); It != contourList.end(); It++ ) {
        otherContour = *It;
        DexelContourBox otherBox ( *otherContour );
        potential = myBox.OverlapAmount ( otherBox );
        if ( ( potential > 0.000000001 && contour == NULL ) ||
             ( potential > overlap && contour != NULL ) ) {
            overlap = potential;
            contour = *It;
        }
    }
    return contour;
}

void DexelContour::CopyDexelPair ( Dexel * dexel, unsigned int index )
{
    InsertAt ( dexel->value, index );
    InsertAt ( dexel->next->value, index );
}

void DexelContour::InsertAt ( double value, unsigned int index )
{
    bool isLeft;
    Dexel * cursor = contour[index];
    if ( ! cursor ) {
        contour[index] = new Dexel ( value );
        return;
    }

    Dexel * previous;
    while ( cursor && cursor->value <= value ) {
        previous = cursor;
        cursor = cursor->next;
    }

    Dexel * dexel = new Dexel ( value );
    if ( cursor ) {
        // Insert before
        dexel->next = cursor;
        dexel->prev = cursor->prev;
        if ( cursor->prev ) {
            cursor->prev->next = dexel;
        }
        cursor->prev = dexel;
        if ( cursor == contour[index] ) {
            contour[index] = dexel;
        }
    }
    else {
        // Append to end
        previous->next = dexel;
        dexel->prev = previous;
    }

    // Alternate assigning left and right
    isLeft = true;
    for ( cursor = contour[index]; cursor; cursor = cursor->next ) {
        cursor->SetLeft ( isLeft );
        isLeft = !isLeft;   // toggle
    }
}

void DexelContour::SpliceContours ( DexelContour& other )
{
    int i;
    for ( i = 0; i < other.contour.size(); i++ ) {
        Dexel * dexel = other.contour[i];
        while ( dexel ) {
            InsertAt ( dexel->value, i );
            dexel = dexel->next;
        }
    }
}

bool DexelContour::IsEmpty ( ) const
{
    unsigned int i;
    for ( i = 0; i < contour.size ( ); i++ ) {
        if ( contour[i] != NULL ) {
            return false;
        }
    }
    return true;
}

void DexelContour::DebugOut ( ) const
{
    Dexel * dexel;
    unsigned int i;
    for ( i = 0; i < contour.size ( ); i++ ) {
        cout << "Y" << i;
        dexel = contour[i];
        while ( dexel ) {
            cout << " " << dexel->value << (dexel->IsLeft() ? "L" : "R");
            dexel = dexel->next;
        }
        cout << endl;
    }
}

DexelContourBox::DexelContourBox ( const DexelContour& contour )
{
    bool first = true;
    DexelContourIterator It ( contour );
    DexelContourPoint point;
    while ( ! It.IsDone() ) {
        point = It.Next();
        if ( first ) {
            minx = point.x;
            maxx = point.x;
            miny = point.y;
            maxy = point.y;
            first = false;
        }
        else {
            minx = min ( point.x, minx );
            maxx = max ( point.x, minx );
            miny = min ( point.y, miny );
            maxy = max ( point.y, maxy );
        }
    }
}

double DexelContourBox::OverlapAmount ( const DexelContourBox& other )
{
    if ( other.minx > this->maxx || this->minx > other.maxx ||
         other.miny > this->maxy || this->miny > other.maxy ) {
        return 0.0;
    }
    return fabs ( ( min ( this->maxx, other.maxx ) - max ( this->minx, other.minx ) ) *
                  ( min ( this->miny, other.miny ) - max ( this->maxy, other.maxy ) ) );
}

DexelContourIterator::DexelContourIterator ( const DexelContour& contour )
 : dexelContour ( contour )
{
    row = 0;
    done = false;
    start = NULL;
    cursor = NULL;
}

void DexelContourIterator::SetStart ( DexelContourPoint& point )
{
    SetCursor ( point );
    start = cursor;
}

void DexelContourIterator::SetCursor ( DexelContourPoint& point )
{
    row = point.y;
    done = false;

    Dexel * dexel;
    for ( dexel = dexelContour.contour[row]; dexel != NULL; dexel = dexel->next ) {
        if ( dexel->value == point.x ) {
            break;
        }
    }
    cursor = dexel;
}

DexelContourPoint DexelContourIterator::Prev ( )
{
    DexelContourPoint rtn;
    unsigned int i;
    Dexel * overlap;

    if ( ! done ) {

        if ( ! cursor ) {
            for ( i = 0; i < dexelContour.contour.size ( ); i++ ) {
                if ( dexelContour.contour[i] != NULL ) {
                    row = i;
                    cursor = dexelContour.contour[i];
                    start = cursor;
                    break;
                }
            }
        }
        else {
            if ( cursor->IsRight() ) {
                overlap = dexelContour.RightmostOfOverlapBelow ( cursor, row );
                if(overlap) {
                    if ( cursor->next && cursor->next->IsPartiallyOverlappedBy ( *overlap->prev ) ) {
                        cursor = cursor->next;
                    } else {
                        cursor = overlap;
                        row--;
                    }
                } else {
                    cursor = cursor->prev;
                }
            }
            else {
                overlap = dexelContour.LeftmostOfOverlapAbove ( cursor, row );
                if (overlap) {
                    if ( cursor->prev && cursor->prev->prev->IsPartiallyOverlappedBy ( *overlap ) ) {
                        cursor = cursor->prev;
                    } else {
                        cursor = overlap;
                        row++;
                    }
                } else {
                    cursor = cursor->next;
                }
            }

            if ( cursor == start ) {
                done = true;
            }
        }
    }

    rtn.Set ( cursor->value, row );
    return rtn;
}

DexelContourPoint DexelContourIterator::Next ( )
{
    DexelContourPoint rtn;
    unsigned int i;
    Dexel * overlap;

    if ( ! done ) {

        if ( ! cursor ) {
            for ( i = 0; i < dexelContour.contour.size ( ); i++ ) {
                if ( dexelContour.contour[i] != NULL ) {
                    row = i;
                    cursor = dexelContour.contour[i];
                    start = cursor;
                    break;
                }
            }
        }
        else {
            if ( cursor->IsLeft() ) {
                overlap = dexelContour.LeftmostOfOverlapBelow ( cursor, row );
                if(overlap) {
                    if ( cursor->prev && cursor->prev->prev->IsPartiallyOverlappedBy ( *overlap ) ) {
                        cursor = cursor->prev;
                    } else {
                        cursor = overlap;
                        row--;
                    }
                } else {
                    cursor = cursor->next;
                }
            }
            else {
                overlap = dexelContour.RightmostOfOverlapAbove ( cursor, row );
                if (overlap) {
                    if ( cursor->next && cursor->next->IsPartiallyOverlappedBy ( *overlap->prev ) ) {
                        cursor = cursor->next;
                    } else {
                        cursor = overlap;
                        row++;
                    }
                } else {
                    cursor = cursor->prev;
                }
            }

            if ( cursor == start ) {
                done = true;
            }
        }
    }

    rtn.Set ( cursor->value, row );
    return rtn;
}
