#ifndef DECIMAL_H
#define DECIMAL_H

/* The Decimal type behind func_02071644 (increment) and its family:
   func_02071364 (multiply), func_020712a0 (build from digit string) and
   func_020715e0 (truncate/round). Repeated by hand, identically, in each of
   those four files before this header; func_02071510 (build from an
   unsigned long long) keeps its own local copy, since it is not one of the
   four whose declarations of func_02071644 this header makes coherent.

   DELIBERATELY DEPENDENCY-FREE, mirroring OamAttr.h: every field is a raw C
   type (unsigned char, char, short), never a project typedef, so this header
   needs no #include and cannot collide with a file-scope typedef any of its
   includers carries. */

typedef struct Decimal
{
  unsigned char sign;
  char unused;
  short exp;
  struct
  {
    unsigned char length;
    unsigned char text[32];
    unsigned char unused;
  } sig;
} Decimal;

#endif
