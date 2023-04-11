#include "main.h"

Image transpose(Image a){
    /* img_sc() will return a copy of the given image*/
    Image b = img_sc(a);
    b.dimX = a.dimY;
    b.dimY = a.dimX;

    for(int x=0;x<(int)a.dimX;x++){
        for(int y=0;y<(int)a.dimY;y++){
            b.data[x*b.dimX+y] = a.data[y*a.dimX+x];
        }
    }

    return b;
}

Image apply_gb(Image a, FVec gv)
{
    // modify here
    Image b = gb_h(a, gv);
    b = transpose(b);
    Image c = gb_h(b,gv);
    c = transpose(c);
    free(b.data);
    return c;
}
