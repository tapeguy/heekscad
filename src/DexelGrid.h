#pragma once

#include "DexelContour.h"


// array of dexels in a common plane
class DexelGrid
{
public:

    typedef enum {
        ACROSS_COLUMN,
        ACROSS_ROW
    } DexelGroupMode;

private:

    Dexel** dexelGrid;
    double colOffset;
    double rowOffset;
    unsigned int colSize;
    unsigned int rowSize;

public:

    DexelGrid ( );
    ~DexelGrid ( );

    void ResizeTo ( unsigned int colSize, unsigned int rowSize );

    void Destroy ( );

    void InsertAt ( double value, unsigned int col, unsigned int row );

    Dexel * GetDexelAt ( unsigned int col, unsigned int row );

    // Verify every dexel has a left and right
    void JoinUnevenPairs ( );

    void GroupDexelsIntoContours ( DexelGroupMode mode,
                                   unsigned int colOrRow,
                                   ContourList& solid_contours,
                                   ContourList& hollow_contours );

    void DebugOut ( );

    double GetColOffset ( ) const
    {
        return colOffset;
    }

    void SetColOffset ( double colOffset )
    {
        this->colOffset = colOffset;
    }

    unsigned int GetColSize ( ) const
    {
        return colSize;
    }

    void SetColSize ( unsigned int colSize )
    {
        this->colSize = colSize;
    }

    double GetRowOffset ( ) const
    {
        return rowOffset;
    }

    void SetRowOffset ( double rowOffset )
    {
        this->rowOffset = rowOffset;
    }

    unsigned int GetRowSize ( ) const
    {
        return rowSize;
    }

    void SetRowSize ( unsigned int rowSize )
    {
        this->rowSize = rowSize;
    }

private:

    bool MergeDexelIntoContours ( Dexel * dexel,
                                  unsigned int offset,
                                  unsigned int max,
                                  ContourList& contours );
};
