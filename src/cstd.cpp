//cpp
/* cstd -- the division-unit routines (mod/div/fdiv/ldiv plus the
 * async/result pairs) and the square-root-unit helpers that live in the
 * same contiguous run, arm9 .text 0x02052ef4..0x02053320, 14 functions.
 *
 * mwccarm emits .text in reverse source order here, so definitions run from
 * the highest address (func_02053274) down to the lowest (mod); each earlier
 * definition is the declaration its callers need, with the remaining
 * callees forward-declared below.
 *
 * leftovers:
 * - cstd::fdiv_async / cstd::reciprocal_async keep the shards' extern "C"
 *   spellings of their mangled names: the real signatures take Fix12<int>
 *   by value, and a class-typed by-value parameter the body reads is
 *   stack-homed under mwccarm (wall 6az, notes/mwccarm-codegen.md). The
 *   mangled symbols come out identical either way; fdiv/ldiv/func_02053200/
 *   func_02053130 call them through the same spellings.
 */
#include "types.h"
#include "nitro/hw/registers.h"

namespace cstd {
int mod(int a, int b);
int div(int a, int b);
int fdiv_result(void);
long long ldiv_result(void);
long long ldiv(int a, int b);
int fdiv(int numerator, int denominator);
}

extern "C" {
void _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(int a, int b);
void _ZN4cstd16reciprocal_asyncE5Fix12IiE(int a);
int _ZN4cstd11fdiv_resultEv(void);
long long _ZN4cstd11ldiv_resultEv(void);
int func_02052fdc(void);
void func_02053008(int n);
}

// @symbol func_02053274
/* Squared distance |a-b|^2 through the hardware sqrt unit; returns the
   20.12 result. */
extern "C" {
int func_02053274(int *a, int *b)
{
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile unsigned short *sqrtcnt = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *sqrtParam = (volatile unsigned int *)0x40002b8;
    int dx = a[0] - b[0];
    long long sq = (long long)dx * dx;
    int dy = a[1] - b[1];
    int dz;
    unsigned short savedIME;
    sq += (long long)dy * dy;
    dz = a[2] - b[2];
    sq += (long long)dz * dz;
    sq <<= 2;
    savedIME = *imeReg;
    *imeReg = 0;
    *sqrtcnt = 1;
    sqrtParam[0] = (unsigned int)sq;
    sqrtParam[1] = (unsigned int)(sq >> 32);
    *imeReg;
    *imeReg = savedIME;
    while (*sqrtcnt & 0x8000);
    return (*(volatile int *)0x40002b4 + 1) >> 1;
}
}

// @symbol _ZN4cstd4fdivEii
namespace cstd {

s32 fdiv(s32 numerator, s32 denominator)
{
    _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(numerator, denominator);
    return _ZN4cstd11fdiv_resultEv();
}

}

// @symbol _ZN4cstd4ldivEii
namespace cstd {

long long ldiv(int a, int b)
{
  _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(a, b);
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile long long*)0x40002a0;
}

}

// @symbol func_02053200
extern "C" {
s32 func_02053200(s32 x)
{
    _ZN4cstd16reciprocal_asyncE5Fix12IiE(x);
    return _ZN4cstd11fdiv_resultEv();
}
}

// @symbol func_020531a4
extern "C" {
int func_020531a4(int a)
{
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile int *sqrtParam = (volatile int *)0x40002b8;
    unsigned short savedIME;
    if (a <= 0) return 0;
    savedIME = *imeReg;
    *imeReg = 0;
    *(volatile unsigned short *)0x40002b0 = 1;
    sqrtParam[0] = 0;
    sqrtParam[1] = a;
    *imeReg;
    *imeReg = savedIME;
    return func_02052fdc();
}
}

// @symbol func_02053130
extern "C" {
s32 func_02053130(s32 x)
{
    s64 quotient;

    if (x > 0) {
        _ZN4cstd16reciprocal_asyncE5Fix12IiE(x);
        func_02053008(x);
        quotient = _ZN4cstd11ldiv_resultEv();

        while (REG_SQRTCNT & SQRT_CONTROL_BUSY)
            ;

        return (s32)((quotient * (s64)REG_SQRT_RESULT + (s64)0x200 * (s64)(1LL << 32)) >>
                     (32 + 10));
    }

    return 0;
}
}

// @symbol _ZN4cstd11ldiv_resultEv
namespace cstd {

long long ldiv_result(void)
{
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile long long*)0x40002a0;
}

}

// @symbol _ZN4cstd11fdiv_resultEv
namespace cstd {

int fdiv_result(void)
{
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return (int)((*(volatile long long*)0x40002a0 + 0x80000) >> 20);
}

}

// @symbol _ZN4cstd16reciprocal_asyncE5Fix12IiE
extern "C" {
void _ZN4cstd16reciprocal_asyncE5Fix12IiE(int a){
  *(volatile unsigned short*)0x4000280 = 1;
  *(volatile unsigned long long*)0x4000290 = ((unsigned long long)0x1000) << 32;
  *(volatile unsigned long long*)0x4000298 = (unsigned)a;
}
}

// @symbol func_02053008
extern "C" {
void func_02053008(int n)
{
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile unsigned short *sqrtcnt = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *sqrtParam = (volatile unsigned int *)0x40002b8;
    unsigned short savedIME;
    if (n > 0) {
        savedIME = *imeReg;
        *imeReg = 0;
        *sqrtcnt = 1;
        sqrtParam[0] = 0;
        sqrtParam[1] = n;
        *imeReg;
        *imeReg = savedIME;
    } else {
        savedIME = *imeReg;
        *imeReg = 0;
        *sqrtcnt = 1;
        sqrtParam[0] = 0;
        sqrtParam[1] = 0;
        *imeReg;
        *imeReg = savedIME;
    }
}
}

// @symbol func_02052fdc
extern "C" {
int func_02052fdc(void) {
    while (*(volatile unsigned short *)0x40002b0 & 0x8000)
        ;
    return (*(volatile unsigned int *)0x40002b4 + 0x200) >> 10;
}
}

// @symbol _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE
extern "C" {
void _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(int a, int b){
  *(volatile unsigned short*)0x4000280 = 1;
  *(volatile unsigned long long*)0x4000290 = ((unsigned long long)(unsigned)a) << 32;
  *(volatile unsigned long long*)0x4000298 = (unsigned)b;
}
}

// @symbol _ZN4cstd3divEii
namespace cstd {

int div(int a, int b)
{
  *(volatile unsigned short*)0x4000280 = 0;
  *(volatile int*)0x4000290 = a;
  *(volatile unsigned long long*)0x4000298 = (unsigned)b;
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile int*)0x40002a0;
}

}

// @symbol _ZN4cstd3modEii
namespace cstd {

int mod(int a, int b)
{
  *(volatile unsigned short*)0x4000280 = 0;
  *(volatile int*)0x4000290 = a;
  *(volatile unsigned long long*)0x4000298 = (unsigned)b;
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile int*)0x40002a8;
}

}
