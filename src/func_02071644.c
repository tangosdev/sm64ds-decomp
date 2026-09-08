// MSL decimal-conversion round-up: increment the last digit of a decimal
// significand and propagate the carry down the digit string. Sibling of
// func_02071510 (which builds the Decimal) and func_020715e0 (which calls this
// one after func_02071698 decides the value rounds up).
//
// ONE FUNCTION, NOT TWO. mwccarm always appends a `bx lr` after a `for (;;)`
// whose exits are all early returns, and here that dead epilogue is the four
// bytes at 0x02071694 that config had carved off as its own function
// "func_02071694", matched as an empty `void f(void) {}` body. Nothing in the
// tree references 0x02071694: config/**/relocs.txt records zero destinations at
// that address, while 0x02071698 (the real next function) has one. The
// compiler's own output settles the extent -- this source emits 0x54 bytes that
// reproduce 0x02071644..0x02071698 exactly -- so the symbol is grown to 0x54
// and the phantom row is deleted. See notes/mwccarm-codegen.md 9a(3), which
// records this pair and func_02072168 / func_020729e8 as the same shape.
//
// recovered: real C, no asm hatch. This file used to carry a HAND-ASM banner
// claiming a "regperm floor"; the floor was the truncated symbol, not the
// register allocator. The draft in notes 9a(3) reached the same bytes through
// a void* with a hand-written +5 and a 6g launder cast on the exponent; neither
// is needed once the Decimal struct is spelled out, so this version carries no
// magic offsets and no launder.
#include "types.h"

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

void func_02071644(Decimal *d, int len)
{
  unsigned char *base = d->sig.text;
  unsigned char *p = base + len - 1;
  for (;;)
  {
    unsigned char c = *p;
    if (c < 9)
    {
      *p = c + 1;
      return;
    }
    if (p == base)
    {
      *p = 1;
      d->exp++;
      return;
    }
    *p-- = 0;
  }
}
