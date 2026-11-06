#ifndef DECIMAL_H
#define DECIMAL_H

/* MSL's decimal type (and its decform, below), behind the conversions of
   src/lib/msl/ansi_fp.c (__ull2dec, __timesdec, __str2dec, __two_exp,
   __rounddec, __dorounddecup, __must_round, __num2dec) and printf.c's
   float2str and round_decimal.
   Before ansi_fp.c was one file, __timesdec carried this exact layout as a
   file-scope copy while __str2dec and __rounddec carried flattened partial
   views of the same bytes (`struct S`, `struct T`), which is what made their
   declarations of __dorounddecup disagree with its definition. float2str
   kept a local `decimal` with a 36-byte text until it took this type.

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

/* MSL's decform, the format __num2dec converts to: __num2dec reads digits
   as the significant-digit count (capped at 32). float2str and double2hex
   fill one on the stack (float2str: style 0, 32 digits). */
typedef struct decform {
    char style;
    char unused;
    short digits;
} decform;

#endif
