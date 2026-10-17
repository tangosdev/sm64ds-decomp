//cpp
/* cstd6 -- second cstd string/printf-band TU, arm9 0x02070348..0x020707a4,
 * 7 functions: cstd::strchr, cstd::strcmp, cstd::strncpy, cstd::strlen +
 * 3 C helpers (rand-LCG, idx-dispatcher, strcpy-like). Below the run,
 * func_0206fd6c (parse_format) stays a one-function source: mwccarm keeps its
 * local print_format struct in registers under -lang c++ while the ROM spills
 * it to the stack. Above, func_020707a4 begins exactly where the run ends.
 */
// leftovers: the three helpers keep address names (no ROM method evidence).
// func_020706b0's alignment preamble and word-at-a-time copy stay as built
// (even the no-op u64 mask colours two words). strcmp keeps MSL's two
// scalar_reload gotos. Callers of func_0207037c sit in the same 0x0206 band,
// so nothing here belongs to a neighbour's file.
#include "types.h"

extern "C" {
extern unsigned int data_0209a624;
extern int func_020589d4(void *p);
extern void func_02058b08(void *p);
extern void func_02058a94(void *p);
extern int func_0206dbf8(int a);
extern int data_020aa0ac;
extern char *data_020a6134[];
extern struct G { char p[0x1c]; int v; } data_020a9fd8;
extern struct G data_020a9ffc;
extern int data_020aa3d4[];

typedef void (*PFN)(int);
}

// @symbol _ZN4cstd6strlenEPKc
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
namespace cstd {

unsigned int strlen(const char *s)
{
  int n = -1;
  unsigned char c;
  do {
    c = *s++;
    n = n + 1;
  } while (c != 0);
  return n;
}

}

// @symbol func_020706b0
extern "C" {
char *func_020706b0(char *dst, const char *src) {
    u8 *s = (u8 *)src;
    u8 *d = (u8 *)dst;
    u8 *d2 = (u8 *)dst;
    u32 a;
    if (((u32)dst & 3) == ((u32)src & 3)) {
        a = ((u32)src & 3) & 0xFFFFFFFFFFFFFFFFull;
        if (a != 0) {
            *d = *s;
            if (*d == 0) return dst;
            a = 3 - a;
            if (a != 0) {
                do {
                    *++d2 = *++s;
                    if (*d2 == 0) return dst;
                } while (--a);
            }
            d2++;
            s++;
        }
        {
            u32 w = *(u32 *)s;
            if (((w + 0xfefefeffu) & 0x80808080u) == 0) {
                d2 -= 4;
                do {
                    d2 += 4;
                    *(u32 *)d2 = w;
                    s += 4;
                    w = *(u32 *)s;
                } while (((w + 0xfefefeffu) & 0x80808080u) == 0);
                d2 += 4;
            }
        }
    }
    *d2 = *s;
    if (*d2 == 0) return dst;
    do {
        *++d2 = *++s;
    } while (*d2 != 0);
    return dst;
}
}

// @symbol _ZN4cstd7strncpyEPcPKcj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
namespace cstd {

char *strncpy(char *dst, const char *src, unsigned int n)
{
  char *d = dst;
  const unsigned char *s = (const unsigned char *)src;
  if (n == 0) return dst;
  do {
    unsigned char c = *s++;
    char *p = d;
    *d++ = c;
    if (*(unsigned char *)p == 0) {
      while (--n) *d++ = 0;
      return dst;
    }
  } while (--n);
  return dst;
}

}

// @symbol _ZN4cstd6strcmpEPKcS1_
namespace cstd {
int strcmp(const char *a, const char *b) {
    unsigned int wb, wa, t;
    unsigned int cnt;
    unsigned int align_a, align_b;
    int diff;
    unsigned char c3, cb, ca;

    ca = (unsigned char)*a;
    cb = (unsigned char)*b;
    diff = (int)ca - (int)cb;
    if (diff != 0)
        return diff;

    align_a = (unsigned int)a & 3;
    align_b = (unsigned int)b & 3;
    if (align_b == align_a) {
        if (align_a != 0) {
            if (ca == 0)
                return 0;
            for (cnt = 3 - align_a; cnt != 0; cnt--) {
                c3 = *(unsigned char *)(++a);
                cb = *(unsigned char *)(++b);
                diff = (int)c3 - (int)cb;
                if (diff != 0)
                    return diff;
                if (c3 == 0)
                    return 0;
            }
            ++a;
            ++b;
        }

        wa = *(unsigned int *)a;
        t = (wa + 0xfefefeff) & 0x80808080;
        wb = *(unsigned int *)b;
        /* scalar_reload gotos kept, as in MSL's strcmp: a while (t == 0) loop shrinks it
           (0x12c) and break + retest grows it (0x13c) */
        if (t != 0)
            goto scalar_reload;
        while (wa == wb) {
            wa = *(unsigned int *)(a += 4);
            wb = *(unsigned int *)(b += 4);
            t = (wa + 0xfefefeff) & 0x80808080;
            if (t != 0)
                goto scalar_reload;
        }
        return (wa > wb) ? 1 : -1;

    scalar_reload:
        ca = *(unsigned char *)a;
        cb = *(unsigned char *)b;
        diff = (int)ca - (int)cb;
        if (diff != 0)
            return diff;
    }
    if (ca == 0)
        return 0;
    for (;;) {
        c3 = *(unsigned char *)(++a);
        cb = *(unsigned char *)(++b);
        diff = (int)c3 - (int)cb;
        if (diff != 0)
            return diff;
        if (c3 == 0)
            return 0;
    }
}
}

// @symbol _ZN4cstd6strchrEPKcc
namespace cstd {
char *strchr(const char *s, char ch_in) {
    unsigned char ch = (unsigned char)ch_in;
    unsigned char c = *(const unsigned char *)s;
    const char *p = s + 1;
    if (c != 0) {
        do {
            if (c == ch) return (char *)(p - 1);
            c = *(const unsigned char *)p++;
        } while (c != 0);
    }
    if (ch != 0) return 0;
    return (char *)(p - 1);
}
}

// @symbol func_0207037c
extern "C" {
int func_0207037c(int idx)
{
  int r4;
  if (idx < 1 || idx > 7) return -1;
  if (func_020589d4(&data_020aa0ac) == 0) {
    data_020a9fd8.v = *(int*)(data_020a6134[2] + 0x6c);
    data_020a9ffc.v = 1;
  } else if (data_020a9fd8.v == *(int*)(data_020a6134[2] + 0x6c)) {
    data_020a9ffc.v += 1;
  } else {
    func_02058b08(&data_020aa0ac);
    data_020a9fd8.v = *(int*)(data_020a6134[2] + 0x6c);
    data_020a9ffc.v = 1;
  }
  r4 = data_020aa3d4[idx - 1];
  if (r4 != 1) data_020aa3d4[idx - 1] = 0;
  data_020a9ffc.v -= 1;
  if (data_020a9ffc.v == 0) func_02058a94(&data_020aa0ac);
  if (r4 == 1 || (r4 == 0 && idx == 1)) return 0;
  if (r4 == 0) func_0206dbf8(0);
  ((PFN)r4)(idx);
  return 0;
}
}

// @symbol func_02070348
extern "C" {
unsigned int func_02070348(void)
{
    unsigned int s = data_0209a624 * 0x41c64e6d + 0x3039;
    data_0209a624 = s;
    return (s >> 16) & 0x7fff;
}
}
