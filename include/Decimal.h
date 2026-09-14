#ifndef DECIMAL_H
#define DECIMAL_H

/* The Decimal type behind func_02071644 (increment) and its family:
   func_02071364 (multiply), func_020712a0 (build from a digit string) and
   func_020715e0 (truncate/round). Those four either define or declare
   func_02071644, and this header is the one spelling all four now use for its
   first parameter. Before it, func_02071364 carried this exact layout as a
   file-scope copy while func_020712a0 and func_020715e0 carried flattened
   partial views of the same bytes (`struct S`, `struct T`), which is what made
   their declarations of func_02071644 disagree with its definition.

   func_02071510 (build from an unsigned long long) keeps its own identical
   local copy on purpose: it never names func_02071644, so no declaration there
   is made coherent by sharing the type, and folding it in would put a fifth
   byte-verified function in this slice for no interface gain.

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
