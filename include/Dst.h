#ifndef DST_H
#define DST_H

#include "types.h"

struct Desc;
struct ElemA;
struct ElemB;

/* Runtime model built from a BMD file by func_020462d0 (Model4.cpp). */
struct RuntimeModel {
    Desc *f0;
    ElemA *f4;
    ElemB *f8;
    ElemA *fc;
    int f10;

    void func_020462d0(Desc *h, ElemA *arr);
};

#endif
