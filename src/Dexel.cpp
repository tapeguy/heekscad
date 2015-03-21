#include "Dexel.h"

void Dexel::Destroy ( )
{
    Dexel * cursor = this;
    Dexel * next;
    while (cursor) {
        next = cursor->next;
        delete cursor;
        cursor = next;
    }
}

bool Dexel::IsPartiallyOverlappedBy ( const Dexel& other ) const
{
    if ( ! this->next || ! other.next ) {
        return false;
    }
    double a = other.value;
    double b = other.next->value;
    if ( ( this->value >= a && this->value < b ) ||        // -- OR --
         ( this->next->value > a && this->next->value <= b ) ||
         ( a >= this->value && b <= this->next->value ) ) {
        return true;
    }
    return false;
}

bool Dexel::IsFullyOverlappedBy ( const Dexel& other ) const
{
    if ( ! this->next || ! other.next ) {
        return false;
    }
    double a = other.value;
    double b = other.next->value;
    if ( ( this->value >= a && this->value < b ) &&        // -- AND --
         ( this->next->value > a && this->next->value <= b ) ) {
        return true;
    }
    return false;
}

unsigned int Dexel::CountEnds ( ) const
{
    const Dexel * cursor = this;
    unsigned int count;
    for ( count = 0; cursor; count++ ) {
        cursor = cursor->next;
    }
    return count;
}
