#pragma once

#include "stdafx.h"

class Dexel
{
private:
    bool isLeft;

public:

    // Doubly linked-list
    Dexel * prev;
    Dexel * next;

    double value;       // one end of a dexel segment


    Dexel ( double value )
    {
        this->value = value;
        prev = NULL;
        next = NULL;
        isLeft = true;
    }

    Dexel ( const Dexel * dexel )
    {
        this->value = dexel->value;
        prev = NULL;
        next = NULL;
        this->isLeft = dexel->isLeft;
    }

    void Destroy ( );

    void SetLeft ( bool left) { this->isLeft = left; };
    bool IsLeft ( ) { return isLeft; };
    bool IsRight ( ) { return !isLeft; };

    bool IsPartiallyOverlappedBy ( const Dexel& other ) const;

    bool IsFullyOverlappedBy ( const Dexel& other ) const;

    unsigned int CountEnds ( ) const;
};
