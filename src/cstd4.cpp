//cpp
/* cstd tail -- MSL runtime helpers, 0x0206dab4..0x0206ece0.
 * __builtin_trap through __pformatter. The five CP15 coprocessor functions
 * ahead of this run live in src/engine/cp15/CP154.cpp. */
/* leftovers: float2str..long2str (0x0206ece0..0x0206fd6c) stay one-function
 * sources. They match under -lang c++, but enrolling them is unresolvable:
 * float2str calls func_01ff9e2c and double2hex calls _deq, both inner
 * addresses of the enrolled itcm asm blob func_01ff97d8.o, which exports only
 * its outer symbol. A TU claims one contiguous .text span, so those two walls
 * stop the absorb before round_decimal/longlong2str/long2str as well.
 * parse_format is C-only under -lang c++ and stays a one-function source;
 * cstd6 (0x02070348..0x020707a4) sits beyond it. Whole-function asm is
 * emitted in source order; compiled bodies are too, under a
 * per-function defer_codegen off. */
#include "types.h"
#include "cstd.h"
#include "nitro/os/thread.h"

extern "C" {
// HAND-ASM PRIMITIVE: `swp` atomic swap has no C equivalent under mwccarm, so asm is the source.
// Store value to *lock and return the word it held. The isdoverlay table
// edits (func_0206cf98, func_0206d32c) spin on it with 0x80 to take a lock.
asm u32 func_0206dab4(u32 value, void *lock) { swp r0, r0, [r1]; bx lr }
}

#pragma push
#pragma defer_codegen off
// HAND-ASM PRIMITIVE: compiler trap builtin; the asm block is the source.
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
extern int OS_TryLockMutex(void *p);
extern void OS_LockMutex(void *p);
extern void OS_UnlockMutex(void *p);
typedef struct S6e06c S6e06c;
int fflush(S6e06c *p);
extern void func_02071784(void);

extern int data_020aa020;
extern int data_020a9fd8;
extern int data_020a9ffc;
extern volatile int __atexit_curr_func;
extern void (*volatile __atexit_funcs[])(void);
extern void (*__console_exit)(void);

/* MSL __exit, statement for statement with Spirit Tracks' copy of
   abort_exit_arm_eabi.c: run __atexit_funcs newest first inside the
   inlined atexit_funcs_access critical region, call __console_exit once,
   fflush(NULL), then leave through func_02071784, which has
   the place and shape of MSL's _ExitProcess (a tail call to the SYS_EXIT
   semihosting call func_0207199c). That file's five globals sit in .bss at
   0x020a9ec8..0x020a9fd8 in reverse declaration order: __console_exit,
   __stdio_exit, __atexit_curr_func, __aborting, then __atexit_funcs[64]. */
void __exit(int status)
{
    if (OS_TryLockMutex(&data_020aa020) == 0) {
        data_020a9fd8 = data_020a6134.current->id;
        data_020a9ffc = 1;
    } else if (data_020a9fd8 == data_020a6134.current->id) {
        data_020a9ffc++;
    } else {
        OS_LockMutex(&data_020aa020);
        data_020a9fd8 = data_020a6134.current->id;
        data_020a9ffc = 1;
    }

    while (__atexit_curr_func > 0) {
        int n = __atexit_curr_func - 1;
        __atexit_curr_func = n;
        __atexit_funcs[n]();
    }

    data_020a9ffc--;
    if (data_020a9ffc == 0) {
        OS_UnlockMutex(&data_020aa020);
    }
    if (__console_exit != 0) {
        __console_exit();
        __console_exit = 0;
    }
    fflush(0);
    func_02071784();
}
#pragma pop

#pragma push
#pragma defer_codegen off
typedef void (*exit_proc)(void);

extern int __aborting;
extern exit_proc __stdio_exit;
void __destroy_global_chain(void);

/* MSL exit: unless abort is under way (__aborting),
   destroy the static objects (__destroy_global_chain) and call
   __stdio_exit once; then __exit(status). */
void exit(int status) {
    if (__aborting == 0) {
        __destroy_global_chain();
        if (__stdio_exit != 0) {
            __stdio_exit();
            __stdio_exit = 0;
        }
    }
    __exit(status);
}
#pragma pop

#pragma push
#pragma defer_codegen off
int raise(int sig);
extern int __aborting;

/* MSL abort: raise(SIGABRT), which is 1 here; set __aborting so exit skips
   the static destructors and __stdio_exit; then exit(EXIT_FAILURE). */
void abort(void) {
    raise(1);
    __aborting = 1;
    exit(1);
}
#pragma pop

typedef union Pair {
    double d;
    float f;
    struct { int x, y; } w;
} Pair;
void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags);

#pragma push
#pragma defer_codegen off
float func_0206dc7c(double a, double b, int op) {
    Pair u1, u2, out;
    u1.d = a;
    u2.d = b;
    func_0206de14((int)&out, u1, u2, op);
    return out.f;
}
#pragma pop

#pragma push
#pragma defer_codegen off
double func_0206dcd4(double a, double b, int op) {
    Pair u1, u2, out;
    u1.d = a;
    u2.d = b;
    func_0206de14((int)&out, u1, u2, op);
    return out.d;
}
#pragma pop

#pragma push
#pragma defer_codegen off
float func_0206dd30(float a, float b, int op) {
    Pair u1, u2, out;
    u1.f = a;
    u2.f = b;
    func_0206de14((int)&out, u1, u2, op);
    return out.f;
}
#pragma pop

#pragma push
#pragma defer_codegen off
double func_0206dd7c(double a, int op) {
    Pair u1, out;
    u1.d = a;
    func_0206de14((int)&out, u1, u1, op);
    return out.d;
}
#pragma pop

#pragma push
#pragma defer_codegen off
float func_0206ddcc(float a, int op) {
    Pair u1, out;
    u1.f = a;
    func_0206de14((int)&out, u1, u1, op);
    return out.f;
}
#pragma pop

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
extern unsigned int __ieee_status(unsigned int mask, unsigned int flags);
extern int raise(int sig);

typedef void (*DispatchFn)(int a0, Pair p1, Pair p2, unsigned int flags);

/* data_020aa3f8: a head word, then one handler per exception bit 26..30
   of flags. */
typedef struct FpHandlerTable {
    unsigned int head;
    DispatchFn handler[5];
} FpHandlerTable;
extern void *func_02073238(void);

void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags)
{
  FpHandlerTable *table = (FpHandlerTable *) func_02073238();
  DispatchFn fn;
  if ((flags & 0xc00000) == 0)
  {
    flags |= __ieee_status(0, 0) & 0xc00000;
  }
  switch (flags & 0x7c000000)
  {
    case 0x40000000:
      fn = table->handler[4];
      break;

    case 0x04000000:
      fn = table->handler[0];
      break;

    case 0x10000000:
      fn = table->handler[2];
      break;

    case 0x20000000:
      fn = table->handler[3];
      break;

    case 0x08000000:
      fn = table->handler[1];
      break;

    default:
      fn = 0;
      break;

  }

  if (fn == 0)
  {
    raise(2);
  }
  fn(a0, p1, p2, flags);
}
#pragma pop

#pragma push
#pragma defer_codegen off
/* MSL __flush_all, statement for statement with Spirit Tracks' copy of
   ansi_files.c: walk the three console FILEs of the __files table at
   data_0209a330 (0x4c bytes each), fflush every FILE whose file_kind, bits
   7..9 of the mode word at +4, is not closed, and return -1 if any flush
   fails. fflush(NULL) calls it, as MSL's does. */
extern char data_0209a330[];
typedef struct S6e06c S6e06c;
int fflush(S6e06c *p);

int __flush_all(void) {
    int result = 0;
    char *p = data_0209a330;
    int i = 1;
    do {
        if (((unsigned int)*(int *)(p + 4) << 22) >> 29) {
            if (fflush((S6e06c *)p)) {
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
void __convert_from_newlines(int a, int *b);
void __prep_buffer(char *self);
typedef int (*Fn)(int, int, int *, int);
/* MSL's buffer_io.c: __flush_buffer, __prep_buffer and
   __convert_from_newlines, statement for statement with Spirit Tracks'
   copy. In MSL's FILE terms: +0x00 handle, +0x18 position, +0x1c buffer,
   +0x20 buffer_size, +0x24 buffer_ptr, +0x28 buffer_len, +0x2c
   buffer_alignment, +0x34 buffer_pos, +0x40 write_proc, +0x48 ref_con, and
   the bit tested at +0x04 is mode.binary_io. In Spirit Tracks this file is
   its own TU between arith.c (abs) and file_io.c (fflush), the same
   neighbours it has here; it stays inside this run because a TU claims one
   contiguous span, and cutting it out would leave a fflush..__pformatter
   tail with no name of its own. */
int __flush_buffer(char *thiz, int *out)
{
  char *new_var;
  int diff = (*((int *) (thiz + 0x24))) - (*((int *) (thiz + 0x1c)));
  if (diff != 0)
  {
    int r;
    *((int *) (thiz + 0x28)) = diff;
    if (((((unsigned int) ((*((unsigned int *) (thiz + 4))) << 0x13)) >> 9) >> 22) == 0)
    {
      __convert_from_newlines(*((int *) (thiz + 0x1c)), (int *) (thiz + 0x28));
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
  __prep_buffer(thiz);
  return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
void __prep_buffer(char *self)
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
void __convert_from_newlines(int a, int *b)
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
    char pad_0e[0x0a];
    unsigned int position;   /* 0x18 */
    char pad_1c[0x0c];
    unsigned int buffer_len; /* 0x28 */
} S6e06c;


/* MSL fflush, statement for statement with Spirit Tracks' copy of
   file_io.c. In MSL's FILE terms: b4c is file_kind, b4b io_mode, b8
   io_state, fd the error flag, +0x18 position and +0x28 buffer_len.
   fflush(NULL) flushes every FILE through __flush_all; a FILE in error or
   closed fails; one open for reading succeeds; otherwise write out the
   buffer (__flush_buffer) and reset the state, position and length. */
int fflush(S6e06c *p)
{
    if (p == 0) {
        return __flush_all();
    }

    if (p->fd != 0 || p->b4c == 0)
        return -1;

    if (p->b4b == 1)
        return 0;

    if (p->b8 >= 3)
        p->b8 = 2;

    if (p->b8 == 2)
        p->buffer_len = 0;

    if (p->b8 != 1) {
        p->b8 = 0;
        return 0;
    }

    if (__flush_buffer((char *)p, 0) != 0) {
        p->fd = 1;
        p->buffer_len = 0;
        return -1;
    }
    p->b8 = 0;
    p->position = 0;
    p->buffer_len = 0;
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
int func_0206e218(int a, int b);

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
        cstd::strncpy(dst + total, bp, len);
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

int func_0206e218(int a, int b) {
    void **p = (void **)data_0209a438[2];
    ((void (*)(int, int))p[1])(a, b);
}
#pragma pop

#pragma push
#pragma defer_codegen off
int func_0206e240(unsigned char *p, unsigned char v) {
    if (p == 0) return 0;
    *p = v;
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
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
/* MSL memset: forwards all three args to __fill_mem (func_0206e330) and
 * returns the destination. The return is void *, the pointer the body hands
 * back in r0. */
extern void func_0206e330(void *dst, int val, unsigned int n);
void *func_0206e2f8(void *dst, int val, unsigned int n) { func_0206e330(dst, val, n); return dst; }
#pragma pop

#pragma push
#pragma defer_codegen off
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

extern unsigned char *parse_format(unsigned char *format_string, char **arg, print_format *format);

extern int func_0206e450(char *obj, const void *src, unsigned int n);
extern int __pformatter(int (*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
                        const unsigned char *format_str, char *arg);
extern char *float2str(double x, char *buff_end, print_format format);
extern char *double2hex(double x, char *buff_end, print_format format);
extern char *longlong2str(long long num, char *buff_end, print_format format);
extern char *long2str(long num, char *buff_end, print_format format);

extern unsigned short data_0209a5e4[];
extern char data_0209a5e8[];

#pragma push
#pragma defer_codegen off
#pragma opt_lifetimes off
/* recovered: MSL memset/__fill_mem -- byte head, 32-byte unrolled block,
 * word tail, byte tail. The head and block loops share one counter, which is
 * what colours both into r3; opt_lifetimes off keeps the word counter separate. */
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
unsigned int func_0206e3dc(char *buf, unsigned int len,
                           unsigned int fmtAddr, unsigned int val) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = __pformatter((int (*)(void *, const char *, size_t))func_0206e450, &ctx,
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
/* __pformatter @ 0x0206e4a4 (arm9, size 0x83c)
 * MSL __pformatter: the printf formatting engine. Walks the format string,
 * hands each %-specifier to parse_format, pulls the matching
 * argument off the va_list, renders it with the long/float/hex-float helpers
 * and emits the result through the caller's WriteProc, applying field width,
 * justification and fill. Returns the character count, or -1 if WriteProc fails.
 */
int __pformatter(int (*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
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
        curr_format = (unsigned char *)cstd::strchr((const char *)format_str, '%');

        if (curr_format == 0) {
            num_chars += (n = cstd::strlen((const char *)format_str));
            if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
                return -1;
            break;
        }

        num_chars += (n = curr_format - format_str);
        if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
            return -1;

        format_str = parse_format((unsigned char *)curr_format, &arg, &format);

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
                if (!(buff_ptr = (unsigned char *)longlong2str(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)long2str(long_num, conv_buff + MAX_BUFF, format)))
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
                if (!(buff_ptr = (unsigned char *)longlong2str(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)long2str(long_num, conv_buff + MAX_BUFF, format)))
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
            if (!(buff_ptr = (unsigned char *)float2str(long_double_num, conv_buff + MAX_BUFF, format)))
                goto invalid_conversion;
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'a':
        case 'A':
            if (format.argument_options == long_double_argument)
                long_double_num = *(long double *)((arg += sizeof(long double)) - sizeof(long double));
            else
                long_double_num = *(double *)((arg += sizeof(double)) - sizeof(double));
            if (!(buff_ptr = (unsigned char *)double2hex(long_double_num, conv_buff + MAX_BUFF, format)))
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
                n = cstd::strlen((const char *)buff_ptr);
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
            num_chars += (n = cstd::strlen((const char *)curr_format));
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
