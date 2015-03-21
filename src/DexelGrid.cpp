#include "DexelGrid.h"


DexelGrid::DexelGrid ( )
{
    this->dexelGrid = NULL;
    this->colOffset = 0.0;
    this->rowOffset = 0.0;
    this->colSize = 0;
    this->rowSize = 0;
}

DexelGrid::~DexelGrid ( )
{
    Destroy ( );
}

void DexelGrid::ResizeTo ( unsigned int colSize, unsigned int rowSize )
{
    Destroy ( );
    dexelGrid = new Dexel *[colSize * rowSize];
    this->colSize = colSize;
    this->rowSize = rowSize;

    unsigned int col, row;
    for ( col = 0; col < colSize; col++ ) {
        for ( row = 0; row < rowSize; row++ ) {
            dexelGrid[col * rowSize + row] = NULL;
        }
    }
}

void DexelGrid::Destroy ( )
{
    unsigned int col, row;
    Dexel * cursor;

    if ( dexelGrid ) {
        for ( col = 0; col < colSize; col++ ) {
            for ( row = 0; row < rowSize; row++ ) {
                if ( dexelGrid[col * rowSize + row] != NULL ) {
                    cursor = dexelGrid[col * rowSize + row];
                    cursor->Destroy ( );
                }
            }
        }
        delete[] dexelGrid;
    }
}

void DexelGrid::InsertAt ( double value, unsigned int col, unsigned int row )
{
    Dexel * cursor = GetDexelAt ( col, row );
    if ( ! cursor ) {
        dexelGrid[col * rowSize + row] = new Dexel ( value );
        return;
    }

    Dexel * previous;
    while ( cursor ) {
        if (  fabs ( cursor->value - value ) < 0.000000001 ) {
            // Dexel boundary fell right on a line, discard.
            return;
        }
        else if ( cursor->value > value ) {
            break;
        }
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
        if ( cursor == dexelGrid[col * rowSize + row] ) {
            dexelGrid[col * rowSize + row] = dexel;
        }
    }
    else {
        // Append to end
        previous->next = dexel;
        dexel->prev = previous;
    }
}

Dexel * DexelGrid::GetDexelAt ( unsigned int col, unsigned int row )
{
    if ( col >= colSize || row >= rowSize )
        return NULL;

    return dexelGrid[col * rowSize + row];
}

void DexelGrid::JoinUnevenPairs ( )
{
    bool isLeft;
    unsigned int col, row, count;
    Dexel * cursor;
    Dexel * dexel;
    for ( col = 0; col < colSize; col++ ) {
        for ( row = 0; row < rowSize; row++ ) {
            cursor = GetDexelAt ( col, row );
            count = cursor->CountEnds ( );

            if ( count % 2 == 1 ) {          // Odd number of ends

                if ( count == 1 ) {
                    // Clone the value
                    dexel = new Dexel ( cursor->value );
                    cursor->next = dexel;
                    dexel->prev = cursor;
                }
                else {                      // 3, 5, 7 ...
                    for ( count = count / 2; count > 0; count--) {
                        cursor = cursor->next;
                    }

                    // Delete the middle value
                    cursor->prev->next = cursor->next;
                    cursor->next->prev = cursor->prev;
                    delete cursor;
                }
            }

            // Alternate assigning left and right
            isLeft = true;
            for ( cursor = GetDexelAt ( col, row ); cursor; cursor = cursor->next ) {
                cursor->SetLeft ( isLeft );
                isLeft = !isLeft;   // toggle
            }
        }
    }
}

void DexelGrid::GroupDexelsIntoContours ( DexelGroupMode mode,
                                          unsigned int colOrRow,
                                          ContourList& solid_contours,
                                          ContourList& hollow_contours )
{
    bool contourFound;
    int j, count;
    unsigned int col, row, offset, max;
    Dexel * cursor;
    Dexel * last_dexel;

    ContourList::iterator It;

    ( mode == ACROSS_COLUMN ) ? col = colOrRow : row = colOrRow;
    max = ( mode == ACROSS_COLUMN ) ? rowSize : colSize;

    for ( ( mode == ACROSS_COLUMN ) ? row = 0   : col = 0;
          ( mode == ACROSS_COLUMN ) ? row < max : col < max;
          ( mode == ACROSS_COLUMN ) ? row++     : col++ ) {

        offset = ( mode == ACROSS_COLUMN ) ? row : col;
        cursor = GetDexelAt ( col, row );
        count = cursor ? cursor->CountEnds() : 0;

        for ( j = 0; j < count - 1; j++ ) {

            if ( j % 2 == 0 ) {   // Solid Dexel
                contourFound = MergeDexelIntoContours ( cursor, offset, max, solid_contours );
                if ( ! contourFound ) {
                    DexelContour * new_contour = new DexelContour ( max );
                    new_contour->SetOffset ( ( mode == ACROSS_COLUMN ) ? this->rowOffset : this->colOffset );
                    new_contour->CopyDexelPair ( cursor, offset );
                    solid_contours.push_back ( new_contour );
                }
            }
            else {                // Hollow "Temporary" Dexel
                contourFound = false;
                for ( It = solid_contours.begin(); It != solid_contours.end(); It++ ) {
                    DexelContour * contour = *It;
                    last_dexel = contour->GetDexelRow ( offset - 1 );
                    while ( last_dexel ) {
                        if ( last_dexel->IsFullyOverlappedBy ( *cursor ) ) {
                            DexelContour * new_contour = new DexelContour ( max );
                            new_contour->SetOffset ( ( mode == ACROSS_COLUMN ) ? this->rowOffset : this->colOffset );
                            new_contour->CopyDexelPair ( this->GetDexelAt ( col, row ), offset );
                            hollow_contours.push_back ( new_contour );
                            contourFound = true;
                            break;
                        }
                        last_dexel = last_dexel->next->next;
                    }
                }
                if ( ! contourFound ) {
                    MergeDexelIntoContours ( cursor, offset, max, hollow_contours );
                }
            }

            cursor = cursor->next;
        }
    }

    // TODO: Make sure all hollow dexels are "capped" and valid (before removing points).

//    for ( It = solid_contours.begin(); It != solid_contours.end(); It++ ) {
//        DexelContour * contour = *It;
//        contour->RemoveAmbiguousPoints ( );
//    }
}

bool DexelGrid::MergeDexelIntoContours ( Dexel * cursor,
                                         unsigned int offset,
                                         unsigned int max,
                                         ContourList& contours )
{
    Dexel * last_dexel;
    ContourList::iterator It;
    DexelContour * prev_contour = NULL;

    for ( It = contours.begin(); It != contours.end(); It++ ) {
        DexelContour * contour = *It;
        last_dexel = contour->GetDexelRow ( offset - 1 );
        while ( last_dexel ) {
            if ( cursor->IsPartiallyOverlappedBy ( *last_dexel ) ) {
                if ( ! prev_contour ) {
                    contour->CopyDexelPair ( cursor, offset );
                    prev_contour = contour;
                }
                else {
                    // Merge contour with the previous one
                    prev_contour->SpliceContours ( *contour );
                    It = contours.erase ( It );
                    It--;
                }
                break;
            }
            last_dexel = last_dexel->next->next;
        }
    }

    return (prev_contour != NULL);
}

void DexelGrid::DebugOut ( )
{
    unsigned int col, row;
    Dexel * dexel;

    for ( col = 0; col < colSize; col++ ) {
        cout << "    col: " << col << endl;
        for ( row = 0; row < rowSize; row++ ) {
            dexel = GetDexelAt ( col, row );
            cout << "        row " << row << ":";
            while ( dexel ) {
                cout << " " << dexel->value << (dexel->IsLeft() ? "L" : "R");
                dexel = dexel->next;
            }
            cout << endl;
        }
    }
}
