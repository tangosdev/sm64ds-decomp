//cpp
/* cstd6 -- the MSL C string band plus rand and raise, arm9
 * 0x02070348..0x020707a4, 7 functions: rand (MSL rand.c), raise (MSL
 * signal.c), then MSL_Common string.c's strchr, strcmp, strncpy, strcpy and
 * strlen -- that file's order reversed, its unreferenced strcat, strncat and
 * strncmp dead-stripped, as pret pokediamond's arm9/asm/MSL_Common_string.s
 * lists the same run. Below the run, parse_format stays a one-function source:
 * mwccarm keeps its local print_format struct in registers under -lang c++
 * while the ROM spills it to the stack. Above, copysign begins exactly where
 * the run ends.
 */
// leftovers: rand's seed random_next is MSL's static, declared extern here.
// strcpy's alignment preamble and word-at-a-time copy stay as built
// (even the no-op u64 mask colours two words). strcmp keeps MSL's two
// scalar_reload gotos. Callers of raise sit in the same 0x0206 band,
// so nothing here belongs to a neighbour's file.
#include "types.h"
#include "msl/string.h"
#include "msl/stdlib.h"
#include "nitro/os/mutex.h"
#include "msl/critical_regions.h"

extern unsigned int random_next;
extern void exit(int status);
typedef void (*sig_func)(int);
extern sig_func signal_funcs[];

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

char *strcpy(char *dst, const char *src) {
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

char *strchr(const char *s, int ch_in) {
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

/* MSL raise (signal.c), statement for statement with Spirit Tracks' copy:
   signals 1..7 (__signal_max), the handler table signal_funcs read and
   reset to SIG_DFL (0) unless it is SIG_IGN (1) inside the inlined
   signal_funcs_access critical region; SIG_IGN, or SIG_DFL for SIGABRT
   (1), returns 0, any other SIG_DFL calls exit(0). abort raises 1 and the
   floating-point dispatcher in cstd4 raises 2, SIGFPE. */

int raise(int sig) {
    sig_func signal_func;
    if (sig < 1 || sig > 7)
        return -1;
    if (OS_TryLockMutex(&__cs[signal_funcs_access]) == 0) {
        __cs_id[signal_funcs_access] = OSi_ThreadInfo.current->id;
        __cs_ref[signal_funcs_access] = 1;
    } else if (__cs_id[signal_funcs_access] == OSi_ThreadInfo.current->id) {
        __cs_ref[signal_funcs_access] += 1;
    } else {
        OS_LockMutex(&__cs[signal_funcs_access]);
        __cs_id[signal_funcs_access] = OSi_ThreadInfo.current->id;
        __cs_ref[signal_funcs_access] = 1;
    }
    signal_func = signal_funcs[sig - 1];
    if ((unsigned int)signal_func != 1)
        signal_funcs[sig - 1] = 0;
    __cs_ref[signal_funcs_access] -= 1;
    if (__cs_ref[signal_funcs_access] == 0)
        OS_UnlockMutex(&__cs[signal_funcs_access]);
    if ((unsigned int)signal_func == 1 || (signal_func == 0 && sig == 1))
        return 0;
    if (signal_func == 0)
        exit(0);
    signal_func(sig);
    return 0;
}

int rand(void) {
    unsigned int s = random_next * 0x41c64e6d + 0x3039;
    random_next = s;
    return (s >> 16) & 0x7fff;
}
