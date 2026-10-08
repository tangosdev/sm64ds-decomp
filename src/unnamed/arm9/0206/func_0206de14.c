/* Dispatches to one of five handlers selected by bits 26-30 of flags, then
 * forwards its own arguments unchanged.
 *
 * The two 8-byte struct parameters are load-bearing, not cosmetic. p2 straddles
 * the r3/stack boundary, so the forwarded call has to stage it contiguously
 * across that edge: the ROM writes the pair to [sp-4] and [sp], then reads the
 * register half back with `ldm r2, {r3}`. Modelling the arguments as five ints
 * read through &a0 produces plain per-slot loads instead and is 12 bytes short.
 *
 * Needs mwccarm 2004/b56. Every 1.2 and 2.0 build brackets that below-sp staging
 * with `mov sp, r2` / `add sp, r2, #4` and takes a frame pointer to do it, which
 * is the same pre-2005 codegen difference recorded in notes/mwccarm-codegen.md
 * 6ai for the fBase_c::Process wrappers.
 */
typedef struct { int x, y; } Pair;

extern int func_02073238(void);
extern int func_01ffb008(int a, int b);
extern void func_0207037c(int idx);

typedef void (*DispatchFn)(int a0, Pair p1, Pair p2, unsigned int flags);

void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags)
{
  DispatchFn fn = (DispatchFn) func_02073238();
  if ((flags & 0xc00000) == 0)
  {
    flags |= func_01ffb008(0, 0) & 0xc00000;
  }
  switch (flags & 0x7c000000)
  {
    case 0x40000000:
      fn = *((DispatchFn *) (((char *) fn) + 0x14));
      break;

    case 0x04000000:
      fn = *((DispatchFn *) (((char *) fn) + 4));
      break;

    case 0x10000000:
      fn = *((DispatchFn *) (((char *) fn) + 0xc));
      break;

    case 0x20000000:
      fn = *((DispatchFn *) (((char *) fn) + 0x10));
      break;

    case 0x08000000:
      fn = *((DispatchFn *) (((char *) fn) + 8));
      break;

    default:
      fn = 0;
      break;

  }

  if (fn == 0)
  {
    func_0207037c(2);
  }
  fn(a0, p1, p2, flags);
}
