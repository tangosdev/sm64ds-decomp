//cpp
/* cstd tail -- MSL runtime helpers, 0x0206dab4..0x0206ece0.
 * __builtin_trap through __pformatter. The five CP15 coprocessor functions
 * ahead of this run live in src/engine/cp15/CP154.cpp. */
/* leftovers: func_0206ece0..func_0206fb08 stay one-function sources. They
 * match under -lang c++, but enrolling them is unresolvable: func_0206ece0
 * calls func_01ff9e2c and func_0206f46c calls _deq, both inner addresses of
 * the enrolled itcm asm blob func_01ff97d8.o, which exports only its outer
 * symbol. A TU claims one contiguous .text span, so those two walls stop the
 * absorb before func_0206f338/func_0206f820/func_0206fb08 as well.
 * func_0206fd6c (parse_format) is C-only under -lang c++ and stays a
 * one-function source; cstd6 (0x02070348..0x020707a4) sits beyond it. Whole-
 * function asm is emitted in source order; compiled bodies are too, under a
 * per-function defer_codegen off. */
#include "types.h"

extern "C" {
// HAND-ASM PRIMITIVE: `swp` atomic swap has no C equivalent under mwccarm, so asm is the source.
// @symbol func_0206dab4
asm void func_0206dab4(void) { swp r0, r0, [r1]; bx lr }
}

#pragma push
#pragma defer_codegen off
// HAND-ASM PRIMITIVE: compiler trap builtin; the asm block is the source.
// @symbol _ZN4cstd14__builtin_trapEv
namespace cstd {

void __builtin_trap(void)
{
  asm { dcd 0xe7ffffff }
}

}
#pragma pop

extern "C" {
#pragma push
#pragma defer_codegen off
// @symbol func_0206dac4
extern int func_020589d4(void *p);
extern void func_02058b08(void *p);
extern void func_02058a94(void *p);
typedef struct S6e06c S6e06c;
int func_0206e06c(S6e06c *p);
extern void func_02071784(void);

extern int data_020aa020;
extern int data_020a6134[];
extern int data_020a9fd8;
extern int data_020a9ffc;
extern volatile int data_020a9ed0;
extern void (*volatile data_020a9ed8[])(void);
extern void (*data_020a9ec8)(void);

int func_0206dac4(int a)
{
    if (func_020589d4(&data_020aa020) == 0) {
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    } else if (data_020a9fd8 == *(int *)((char *)data_020a6134[2] + 0x6c)) {
        data_020a9ffc++;
    } else {
        func_02058b08(&data_020aa020);
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    }

    while (data_020a9ed0 > 0) {
        int n = data_020a9ed0 - 1;
        data_020a9ed0 = n;
        data_020a9ed8[n]();
    }

    data_020a9ffc--;
    if (data_020a9ffc == 0) {
        func_02058a94(&data_020aa020);
    }
    if (data_020a9ec8 != 0) {
        data_020a9ec8();
        data_020a9ec8 = 0;
    }
    func_0206e06c(0);
    func_02071784();
}
#pragma pop

#pragma push
#pragma defer_codegen off
typedef void (*FP0206dbf8)(void);

extern int data_020a9ed4;
extern FP0206dbf8 data_020a9ecc;
void func_02072f3c(void);

// @symbol func_0206dbf8
int func_0206dbf8(int a) {
    if (data_020a9ed4 == 0) {
        func_02072f3c();
        if (data_020a9ecc != 0) {
            data_020a9ecc();
            data_020a9ecc = 0;
        }
    }
    return func_0206dac4(a);
}
#pragma pop

#pragma push
#pragma defer_codegen off
void func_0207037c(int a);
extern int data_020a9ed4;

// @symbol func_0206dc4c
void func_0206dc4c(void) {
    func_0207037c(1);
    data_020a9ed4 = 1;
    func_0206dbf8(1);
}
#pragma pop

typedef struct Pair { int x, y; } Pair;
void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags);

// HAND-ASM PRIMITIVE: softfloat glue, double OP double -> float, forwarding to func_0206de14.
// ADS-era library code (below-sp staging, no sp adjustment) no mwccarm build
// reproduces from C.

// @symbol func_0206dc7c
asm float func_0206dc7c(double a, double b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    ldr ip, [sp, #0x28]
    str r2, [sp, #0x10]
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r3, [sp, #0x14]
    str ip, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: softfloat glue, double OP double -> double, forwarding to func_0206de14.
// ADS-era library code (below-sp staging, no sp adjustment) no mwccarm build
// reproduces from C.

// @symbol func_0206dcd4
asm double func_0206dcd4(double a, double b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    ldr ip, [sp, #0x28]
    str r2, [sp, #0x10]
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r3, [sp, #0x14]
    str ip, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    ldr r1, [sp, #0x1c]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: softfloat glue, float OP float -> float, forwarding to func_0206de14.
// ADS-era library code (below-sp staging, no sp adjustment) no mwccarm build
// reproduces from C.

// @symbol func_0206dd30
asm float func_0206dd30(float a, float b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    str r0, [sp, #8]
    str r1, [sp, #0x10]
    str r2, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: softfloat glue, unary double -> double, forwarding to func_0206de14.
// ADS-era library code (below-sp staging, no sp adjustment) no mwccarm build
// reproduces from C.

// @symbol func_0206dd7c
asm double func_0206dd7c(double a, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x1c
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r2, [sp, #4]
    ldr r1, [sp, #8]
    ldr r0, [sp, #0xc]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x10
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x10]
    ldr r1, [sp, #0x14]
    add sp, sp, #0x1c
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: softfloat glue, unary float -> float, forwarding to func_0206de14.
// ADS-era library code (below-sp staging, no sp adjustment) no mwccarm build
// reproduces from C.

// @symbol func_0206ddcc
asm float func_0206ddcc(float a, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x1c
    str r0, [sp, #8]
    str r1, [sp, #4]
    ldr r1, [sp, #8]
    ldr r0, [sp, #0xc]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x10
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x10]
    add sp, sp, #0x1c
    ldmia sp!, {lr}
    bx lr
}

#pragma push
#pragma defer_codegen off
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
extern int func_02073238(void);
extern int func_01ffb008(int a, int b);
extern void func_0207037c(int idx);

typedef void (*DispatchFn)(int a0, Pair p1, Pair p2, unsigned int flags);

// @symbol func_0206de14
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
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206df14
/* Heap-walk helper: consults func_0206e06c for each of three records. */
extern char data_0209a330[];
typedef struct S6e06c S6e06c;
int func_0206e06c(S6e06c *p);

// @symbol func_0206df14
int func_0206df14(void) {
    int result = 0;
    char *p = data_0209a330;
    int i = 1;
    do {
        if (((unsigned int)*(int *)(p + 4) << 22) >> 29) {
            if (func_0206e06c((S6e06c *)p)) {
                result = -1;
            }
        }
        p = (i < 3) ? (data_0209a330 + i++ * 0x4c) : 0;
    } while (p);
    return result;
}
#pragma pop
}

#pragma push
#pragma defer_codegen off
// @symbol _ZN4cstd3absEi
namespace cstd {

int abs(int x)
{
  return x<0 ? -x : x;
}

}
#pragma pop

extern "C" {
#pragma push
#pragma defer_codegen off
void func_0206e068(int a, int *b);
void func_0206e030(char *self);
typedef int (*Fn)(int, int, int *, int);
// @symbol func_0206df90
int func_0206df90(char *thiz, int *out)
{
  char *new_var;
  int diff = (*((int *) (thiz + 0x24))) - (*((int *) (thiz + 0x1c)));
  if (diff != 0)
  {
    int r;
    *((int *) (thiz + 0x28)) = diff;
    if (((((unsigned int) ((*((unsigned int *) (thiz + 4))) << 0x13)) >> 9) >> 22) == 0)
    {
      func_0206e068(*((int *) (thiz + 0x1c)), (int *) (thiz + 0x28));
    }
    r = (*((Fn *) (thiz + 0x40)))(*((int *) thiz), *((int *) (thiz + 0x1c)), (int *) (thiz + 0x28), *((int *) (thiz + 0x48)));
    if (out != 0)
    {
      *out = *((int *) (thiz + 0x28));
    }
    if (r != 0)
    {
      return r;
    }
    new_var = thiz + 0x18;
    *(int *)(new_var) += *((int *) (thiz + 0x28));
  }
  func_0206e030(thiz);
  return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e030
void func_0206e030(char *self)
{
    *(int *)(self + 0x24) = *(int *)(self + 0x1c);
    *(int *)(self + 0x28) = *(int *)(self + 0x20);
    *(int *)(self + 0x28) -=
        (*(int *)(self + 0x18) & *(int *)(self + 0x2c));
    *(int *)(self + 0x34) = *(int *)(self + 0x18);
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e068
void func_0206e068(int a, int *b)
{
}
#pragma pop

#pragma push
#pragma defer_codegen off
typedef struct S6e06c {
    int unk0;
    unsigned int b4a:2;
    unsigned int b4b:3;
    unsigned int b4gap:2;
    unsigned int b4c:3;
    unsigned int b4rest:22;
    unsigned int b8:3;
    unsigned int b8rest:29;
    unsigned char fc;
    unsigned char fd;
} S6e06c;


// @symbol func_0206e06c
int func_0206e06c(S6e06c *p)
{
    if (p == 0) {
        return func_0206df14();
    }

    if (p->fd != 0 || p->b4c == 0)
        return -1;

    if (p->b4b == 1)
        return 0;

    if (p->b8 >= 3)
        *(unsigned int *)(((int)p + 8)) = (*(unsigned int *)(((int)p + 8)) & ~7) | 2;

    if (p->b8 == 2)
        *(int *)((char *)p + 0x28) = 0;

    if (p->b8 != 1) {
        *(unsigned int *)(((int)p + 8)) &= ~7;
        return 0;
    }

    if (func_0206df90((char *)p, 0) != 0) {
        p->fd = 1;
        *(int *)((char *)p + 0x28) = 0;
        return -1;
    }
    *(unsigned int *)(((int)p + 8)) &= ~7;
    *(int *)((char *)p + 0x18) = 0;
    *(int *)((char *)p + 0x28) = 0;
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
int func_0206e218(int a, int b);
extern void _ZN4cstd7strncpyEPcPKcj(char *d, const char *s, u32 n);

// @symbol func_0206e184
u32 func_0206e184(char *dst, u16 *src, u32 max)
{
    u32 total = 0;
    char buf[4];
    char *bp;
    if (dst == 0 || src == 0)
        return 0;
    bp = buf;
    for (;;) {
        u16 code = *src;
        int len;
        if (code == 0) {
            dst[total] = 0;
            break;
        }
        src++;
        len = func_0206e218((int)bp, code);
        if (total + len > max)
            break;
        _ZN4cstd7strncpyEPcPKcj(dst + total, bp, len);
        total += len;
        if (total > max)
            break;
    }
    return total;
}
#pragma pop

#pragma push
#pragma defer_codegen off
extern void *data_0209a438[];

// @symbol func_0206e218
int func_0206e218(int a, int b) {
    void **p = (void **)data_0209a438[2];
    ((void (*)(int, int))p[1])(a, b);
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e240
int func_0206e240(unsigned char *p, unsigned char v) {
    if (p == 0) return 0;
    *p = v;
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e254
int func_0206e254(unsigned short *dst, unsigned char *src, int flag) {
    if (src == 0) return 0;
    if (flag == 0) return -1;
    if (dst != 0) *dst = *src;
    if (*src == 0) return 0;
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e28c
int func_0206e28c(const u8* a, const u8* b, u32 n) {
    while (n != 0) {
        u8 ca = *a++;
        u8 cb = *b++;
        if (ca != cb) {
            if (a[-1] < b[-1]) return -1;
            return 1;
        }
        n--;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e2cc
u8 *func_0206e2cc(u8 *p, int c, unsigned int n)
{
    u8 ch = (u8)c;
    if (n != 0) {
        do {
            if (*p++ == ch) return p - 1;
        } while (--n != 0);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e2f8
/* MSL memset: forwards all three args to __fill_mem (func_0206e330) and
 * returns the destination. The return is void *, the pointer the body hands
 * back in r0. */
extern void func_0206e330(void *dst, int val, unsigned int n);
void *func_0206e2f8(void *dst, int val, unsigned int n) { func_0206e330(dst, val, n); return dst; }
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e310
void *func_0206e310(void *dst, const void *src, unsigned int n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n != 0) {
        *d++ = *s++;
        n--;
    }
    return dst;
}
#pragma pop

/* MSL printf cluster shared layout: print_format is the specifier frame
 * parse_format fills and the render helpers consume by value. */
typedef struct {
    unsigned char justification_options;
    unsigned char sign_options;
    unsigned char precision_specified;
    unsigned char alternate_form;
    unsigned char argument_options;
    unsigned char conversion_char;
    char fill_char;
    char pad7;
    int field_width;
    int precision;
} print_format;

enum { only_minus = 0, sign_always = 1, space_holder = 2 };
enum { left_justification = 0, right_justification = 1, zero_fill = 2 };
enum {
    normal_argument = 0,
    char_argument,
    short_argument,
    long_argument,
    intmax_argument,
    size_t_argument,
    ptrdiff_argument,
    long_long_argument,
    long_double_argument,
    wchar_argument
};

#define MAX_BUFF 512

extern unsigned char *func_0206fd6c(unsigned char *format_string, char **arg, print_format *format); /* parse_format */
extern char *_ZN4cstd6strchrEPKcc(const char *s, char c);
extern unsigned int _ZN4cstd6strlenEPKc(const char *s);

extern int func_0206e450(char *obj, const void *src, unsigned int n);
extern int func_0206e4a4(int (*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
                         const unsigned char *format_str, char *arg);
extern char *func_0206ece0(double x, char *buff_end, print_format format);
extern char *func_0206f46c(double x, char *buff_end, print_format format);
extern char *func_0206f820(long long num, char *buff_end, print_format format);
extern char *func_0206fb08(long num, char *buff_end, print_format format);

extern unsigned short data_0209a5e4[];
extern char data_0209a5e8[];

#pragma push
#pragma defer_codegen off
#pragma opt_lifetimes off
/* recovered: MSL memset/__fill_mem -- byte head, 32-byte unrolled block,
 * word tail, byte tail. The head and block loops share one counter, which is
 * what colours both into r3; opt_lifetimes off keeps the word counter separate. */
// @symbol func_0206e330
void func_0206e330(void *dst, int val, u32 n)
{
    u8 *p = (u8 *)dst;
    u32 c = (u32)val & 0xff;

    if (n >= 0x20) {
        u32 k, w;

        k = (u32)(-(int)p) & 3;
        if (k != 0) {
            n -= k;
            do {
                *p++ = (u8)c;
            } while (--k);
        }

        if (c != 0) {
            c |= (c << 24) | (c << 16) | (c << 8);
        }

        k = n >> 5;
        if (k != 0) {
            do {
                ((u32 *)p)[0] = c;
                ((u32 *)p)[1] = c;
                ((u32 *)p)[2] = c;
                ((u32 *)p)[3] = c;
                ((u32 *)p)[4] = c;
                ((u32 *)p)[5] = c;
                ((u32 *)p)[6] = c;
                ((u32 *)p)[7] = c;
                p += 0x20;
            } while (--k);
        }

        w = (n & 0x1f) >> 2;
        if (w != 0) {
            do {
                *(u32 *)p = c;
                p += 4;
            } while (--w);
        }

        n &= 3;
    }

    if (n != 0) {
        do {
            *p++ = (u8)c;
        } while (--n);
    }
}
#pragma pop

#pragma push
#pragma defer_codegen off
struct Ctx { char *buf; unsigned int len; int z; };
// @symbol func_0206e3dc
unsigned int func_0206e3dc(char *buf, unsigned int len,
                           unsigned int fmtAddr, unsigned int val) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4((int (*)(void *, const char *, size_t))func_0206e450, &ctx,
                      (const unsigned char *)fmtAddr, (char *)val);
    if (buf == 0) return r;
    if (r < len) {
        buf[r] = 0;
        return r;
    }
    if (len != 0) {
        char *end = buf + len;
        end[-1] = 0;
    }
    return r;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e450
int func_0206e450(char *obj, const void *src, unsigned int n)
{
    unsigned int off = *(unsigned int*)(obj + 8);
    unsigned int len = *(unsigned int*)(obj + 4);
    if (off + n > len)
        n = len - off;
    func_0206e310(*(char**)(obj) + off, src, n);
    {
        int *p = (int*)(obj + 8);
        *p = *p + n;
    }
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
/* func_0206e4a4 @ 0x0206e4a4 (arm9, size 0x83c)
 * MSL __pformatter: the printf formatting engine. Walks the format string,
 * hands each %-specifier to parse_format (func_0206fd6c), pulls the matching
 * argument off the va_list, renders it with the long/float/hex-float helpers
 * and emits the result through the caller's WriteProc, applying field width,
 * justification and fill. Returns the character count, or -1 if WriteProc fails.
 */
// @symbol func_0206e4a4
int func_0206e4a4(int (*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
                  const unsigned char *format_str, char *arg)
{
    int num_chars;
    long long long_long_num;
    const unsigned char *curr_format;
    int n;
    int chars_written;
    long long_num;
    long double long_double_num;
    unsigned char *buff_ptr;
    unsigned char fill_char = ' ';
    char blank;
    print_format format;
    char conv_buff[MAX_BUFF];

    num_chars = 0;

    while (*format_str) {
        curr_format = (unsigned char *)_ZN4cstd6strchrEPKcc((const char *)format_str, '%');

        if (curr_format == 0) {
            num_chars += (n = _ZN4cstd6strlenEPKc((const char *)format_str));
            if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
                return -1;
            break;
        }

        num_chars += (n = curr_format - format_str);
        if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
            return -1;

        format_str = func_0206fd6c((unsigned char *)curr_format, &arg, &format);

        switch (format.conversion_char) {
        case 'd':
        case 'i':
            if (format.argument_options == long_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == long_long_argument)
                long_long_num = *(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == intmax_argument)
                long_num = (long)*(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == size_t_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == ptrdiff_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else
                long_num = *(int *)((arg += sizeof(int)) - sizeof(int));

            if (format.argument_options == short_argument)
                long_num = (short)long_num;
            if (format.argument_options == char_argument)
                long_num = (signed char)long_num;

            if (format.argument_options == long_long_argument) {
                if (!(buff_ptr = (unsigned char *)func_0206f820(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)func_0206fb08(long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            }
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'o':
        case 'u':
        case 'x':
        case 'X':
            if (format.argument_options == long_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == long_long_argument)
                long_long_num = *(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == intmax_argument)
                long_num = (long)*(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == size_t_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == ptrdiff_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else
                long_num = *(int *)((arg += sizeof(int)) - sizeof(int));

            if (format.argument_options == short_argument)
                long_num = (unsigned short)long_num;
            if (format.argument_options == char_argument)
                long_num = (unsigned char)long_num;

            if (format.argument_options == long_long_argument) {
                if (!(buff_ptr = (unsigned char *)func_0206f820(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)func_0206fb08(long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            }
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            if (format.argument_options == long_double_argument)
                long_double_num = *(long double *)((arg += sizeof(long double)) - sizeof(long double));
            else
                long_double_num = *(double *)((arg += sizeof(double)) - sizeof(double));
            if (!(buff_ptr = (unsigned char *)func_0206ece0(long_double_num, conv_buff + MAX_BUFF, format)))
                goto invalid_conversion;
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'a':
        case 'A':
            if (format.argument_options == long_double_argument)
                long_double_num = *(long double *)((arg += sizeof(long double)) - sizeof(long double));
            else
                long_double_num = *(double *)((arg += sizeof(double)) - sizeof(double));
            if (!(buff_ptr = (unsigned char *)func_0206f46c(long_double_num, conv_buff + MAX_BUFF, format)))
                goto invalid_conversion;
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 's':
            if (format.argument_options == wchar_argument) {
                unsigned short *ws = *(unsigned short **)((arg += sizeof(void *)) - sizeof(void *));
                if (ws == 0)
                    ws = data_0209a5e4;
                if ((int)func_0206e184(conv_buff, ws, MAX_BUFF) < 0)
                    goto invalid_conversion;
                buff_ptr = (unsigned char *)conv_buff;
            } else {
                buff_ptr = *(unsigned char **)((arg += sizeof(void *)) - sizeof(void *));
            }
            if (buff_ptr == 0)
                buff_ptr = (unsigned char *)data_0209a5e8;
            if (format.alternate_form) {
                n = *buff_ptr++;
                if (format.precision_specified && n > format.precision)
                    n = format.precision;
            } else if (format.precision_specified) {
                unsigned char *e;
                n = format.precision;
                if ((e = (unsigned char *)func_0206e2cc((u8 *)buff_ptr, 0, n)) != 0)
                    n = e - buff_ptr;
            } else {
                n = _ZN4cstd6strlenEPKc((const char *)buff_ptr);
            }
            break;

        case 'n': {
            void *p = *(void **)((arg += sizeof(void *)) - sizeof(void *));
            switch (format.argument_options) {
            case normal_argument:
                *(int *)p = num_chars;
                break;
            case short_argument:
                *(short *)p = num_chars;
                break;
            case long_argument:
                *(long *)p = num_chars;
                break;
            case long_long_argument:
                *(long long *)p = num_chars;
                break;
            }
            continue;
        }

        case 'c':
            conv_buff[0] = (char)*(int *)((arg += sizeof(int)) - sizeof(int));
            buff_ptr = (unsigned char *)conv_buff;
            n = 1;
            break;

        case '%':
            conv_buff[0] = '%';
            buff_ptr = (unsigned char *)conv_buff;
            n = 1;
            break;

        case 0xFF:
        default:
        /* invalid_conversion gotos kept, as in MSL's __pformatter: a failed-conversion
           test after the switch instead grows it (0x848) */
        invalid_conversion:
            num_chars += (n = _ZN4cstd6strlenEPKc((const char *)curr_format));
            if (n && !(*WriteProc)(WriteProcArg, (const char *)curr_format, n))
                return -1;
            return num_chars;
        }

        chars_written = n;

        if (format.justification_options != left_justification) {
            fill_char = (format.justification_options == zero_fill) ? '0' : ' ';

            if ((*buff_ptr == '+' || *buff_ptr == '-' || *buff_ptr == ' ') && fill_char == '0') {
                if (!(*WriteProc)(WriteProcArg, (const char *)buff_ptr, 1))
                    return -1;
                buff_ptr++;
                n--;
            }

            while (chars_written < format.field_width) {
                if (!(*WriteProc)(WriteProcArg, (const char *)&fill_char, 1))
                    return -1;
                chars_written++;
            }
        }

        if (n && !(*WriteProc)(WriteProcArg, (const char *)buff_ptr, n))
            return -1;

        if (format.justification_options == left_justification) {
            while (chars_written < format.field_width) {
                blank = ' ';
                if (!(*WriteProc)(WriteProcArg, &blank, 1))
                    return -1;
                chars_written++;
            }
        }

        num_chars += chars_written;
    }

    return num_chars;
}
#pragma pop
}
