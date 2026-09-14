// Increment the decimal significand, propagating carry into the exponent.
// The 2004/b56 output occupies 0x02071644..0x02071698, including the final bx lr.
// recovered: real C, no asm hatch; the investigation is in notes/mwccarm-codegen.md 9a(3).
#include "types.h"
#include "Decimal.h"

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
