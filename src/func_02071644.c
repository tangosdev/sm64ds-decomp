// MSL decimal-conversion round-up: increment the last digit of a decimal
// significand and propagate the carry down the digit string. Sibling of
// func_02071510 (which builds the Decimal) and func_020715e0 (which calls this
// one after func_02071698 decides the value rounds up).
//
// ONE FUNCTION, NOT TWO, ON THIS BODY'S OWN OUTPUT. Compiled at 2004/b56 this
// source emits 0x54 bytes reproducing 0x02071644..0x02071698 exactly, with no
// relocation slot to wildcard, and its last instruction is the `bx lr` at
// 0x02071694 -- the four bytes config had carved off as a separate function
// "func_02071694" and someone matched with an empty `void f(void) {}`. Compiled
// against the truncated 0x50 the same object is one instruction too long. That
// is what THIS body emits under THIS compiler; it is not a general rule about
// what mwccarm does after a `for (;;)`.
//
// The relocation index points the same way and claims less: no destination in
// config/**/relocs.txt names 0x02071694, while 0x02071698 (the real next
// function) has one. That is an absence of references in that index, not proof
// that execution can never enter the address.
//
// recovered: real C, no asm hatch. This file used to carry a HAND-ASM banner
// claiming a "regperm floor"; the floor was the truncated symbol, not the
// register allocator. The earlier investigation, the sibling split-symbol pair
// func_02072168 / func_020729e8, and the void*-plus-launder draft are in
// notes/mwccarm-codegen.md 9a(3). Neither a magic offset nor a launder is
// needed once the Decimal struct is spelled out, so this version carries
// neither.
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
