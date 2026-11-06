#ifndef MTX43_H
#define MTX43_H

/* One row of a 4x3 fixed-point matrix. */
typedef struct Mtx43Row {
    int x, y, z;
} Mtx43Row;

/* A 4x3 fixed-point matrix, twelve words row by row: r[0..2] are the X, Y and
   Z axis rows and r[3] is the translation. a[] is the same words flat.

   It is a union so that assigning one copies the twelve words as a block;
   a struct of four row structs is copied row by row in C++. The rows are an
   array, not anonymous r0..r3 members, because an anonymous struct takes
   compiler @NNN numbers in every file that includes this header. */
typedef union Mtx43 {
    int a[12];
    Mtx43Row r[4];
} Mtx43;

#endif
