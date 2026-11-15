#ifndef VECTOR3BITS_H
#define VECTOR3BITS_H

/* Vector3's destructor turns a copy into memberwise stores. A struct
   whose only member is an array stays one block move, which is what
   the ROM does for these three ints. */
struct Vector3Bits {
    int w[3];
};

#endif
