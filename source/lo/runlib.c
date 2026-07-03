#include <stdio.h>


zzchk(indx, lo, hi)
int indx, lo, hi;
{
    if (indx < lo || indx > hi)
        panic("Index out of range.  Value=%d.  Range: %d..%d\n", 
            indx, lo, hi);

    return(indx-lo);
}
