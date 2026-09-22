/* HOST COPY of src/func_ov006_020d8408.c, dScMgBomroom_c ("Sort or 'Splode",
 * actor id 0x172, scene 370). Run link100 wave 10 round 2, lane SCENES2.
 *
 * PORT_HOST_ABI: ONE ARGUMENT THAT RIDES THROUGH r1 ON ARM AND IS NOT PUSHED
 * ON THE HOST. Line 202 of the src TU calls a two-parameter body through a
 * ONE-parameter cast:
 *
 *     ((void (*)(char *)) func_ov006_020d66c4)(c);
 *
 * which is the only way to spell, in C, an ARM call whose second argument is
 * already sitting in r1 from the statement above it. Under MSVC that argument
 * is not pushed at all, the callee reads whatever the caller's frame happens to
 * hold as `idx`, and its first act is base + idx * 16 + 0x626c.
 *
 * WHAT IT COSTS, measured headless on this tree at 600 frames with
 * SM64DS_FAULTS_FATAL=1: scene 370 dies at func_ov006_020d66c4+0xc, READ at
 * 0x3879cf6c, with esi and ebx holding the scene object 0x307f9d00 and eax
 * 0x38796d00. 0x38796d00 - 0x307f9d00 is 0x07f97000, so the garbage idx was
 * 0x7f9700, and 0x38796d00 + 0x626c is the faulting address exactly.
 *
 * WHAT r1 ACTUALLY HOLDS, read off capstone over extracted/overlays/
 * overlay_0006.bin at base 0x020bfec0 (the extracted overlay, never the dsd
 * copy, which is shifted). The call is at 0x020d86e8:
 *
 *     020d86d0  str  r1, [r2, #0x670]
 *     020d86d4  ldr  r1, [sp, #0x1c]
 *     020d86d8  str  r1, [r2, #0x688]
 *     020d86dc  bne  #0x20d86ec
 *     020d86e0  strb r5, [r0]
 *     020d86e4  mov  r0, sl
 *     020d86e8  bl   #0x20d66c4
 *
 * `str r1,[r2,#0x688]` is the src line one statement above the call,
 * `*((s32*)((c + (j<<6)) + 0x4688)) = z1;` (r2 is c + (j<<6) + 0x4000), so the
 * word riding in r1 is z1. Nothing between that store and the branch touches
 * r1: the bne is a branch, the strb is `*ptype = one`, and the mov loads r0.
 * And the callee reads it as its second parameter, without question:
 *
 *     020d66c4  push {r4, r5, lr}
 *     020d66c8  sub  sp, sp, #4
 *     020d66cc  mov  r5, r0
 *     020d66d0  mov  r4, r1
 *     020d66d4  add  r0, r5, r4, lsl #4
 *     020d66d8  add  r1, r0, #0x6000
 *     020d66dc  ldrb r0, [r1, #0x26c]
 *
 * THE OTHER FOUR CALL SITES IN THIS FUNCTION ARE NOT AFFECTED and that is the
 * check that makes this one site rather than a class: 0x020d8760 sets r1 with
 * `ldr r1,[sp,#0x24]` and 0x020d8790 with `mov r1,r5`, and the src TU spells
 * both of those with two arguments already. This is the only site that rides.
 *
 * NO HAND TRANSCRIPTION. This file was produced mechanically and a reviewer can
 * reproduce it byte for byte:
 *
 *     sed 's|((void (\*)(char \*)) func_ov006_020d66c4)(c);|func_ov006_020d66c4(c, z1);|' \
 *         src/func_ov006_020d8408.c
 *
 * Everything below this comment is the src TU unchanged.
 *
 * THE DECOMP-SIDE QUESTION IS ROUTED, NOT TAKEN. Whether mwccarm still
 * reproduces the ROM's instruction stream when the call is spelled with two
 * arguments is a byte-gated-tree question and belongs to a decomp lane;
 * port/CMakeLists.txt already records this TU as NONMATCHING with its logic
 * verified, so nothing here risks un-matching a matched body. src/ is not
 * edited, and port/slice_sos.txt drops the src row so the two copies cannot
 * both be taken.
 */
#include "types.h"

extern int RandomIntInternal(int *seed);
extern void func_ov006_020d66c4(char *base, int idx);
extern int data_0209d4b8;
extern u16 data_ov006_0212e2e8[];
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_loop_invariants off
// PORT_HOST_ABI: register ride-through. The ROM calls func_ov006_020d66c4
// at 0x020d86e8 with its second argument already in r1 (`ldr r1,[sp,#0x1c]`
// at 0x020d86d4, which is the z1 store one line above the call), and the src
// TU spells that as a one-parameter cast. The host pushes nothing, so the
// callee indexes by stack garbage. This copy passes z1.
void func_ov006_020d8408(char *c)
{
  s32 sb;
  s32 step;
  s32 ang;
  s32 i;
  s32 one;
  s32 off;
  s32 t0;
  u32 rnd;
  s32 cnt;
  s32 flag;
  s32 j;
  s32 z0;
  s32 z1;
  s32 z2;
  s32 z3;
  s32 v80000;
  s32 new_var;
  s32 v4;
  s32 v200;
  s32 vB8000;
  s32 j0;
  if ((*((u16 *) ((c + 0x6200) + 0xe2))) != 0)
  {
    (*((u16 *) ( ((int) (c + 0x62e2)))))--;
    if ((*((s16 *) ((c + 0x6200) + 0xe2))) < 0)
    {
      *((u16 *) ((c + 0x6200) + 0xe2)) = 0;
    }
    return;
  }
  t0 = *((s32 *) ((c + 0x6000) + 0x2d8));
  sb = 0;
  if (t0 >= 0x12c)
  {
    sb = 0xc;
  }
  else
    if (t0 >= 0xc6)
  {
    sb = 0xb;
  }
  else
    if (t0 >= 0x9f)
  {
    sb = 0xa;
  }
  else
    if (t0 >= 0x84)
  {
    sb = 9;
  }
  else
    if (t0 >= 0x5c)
  {
    sb = 8;
  }
  else
    if (t0 >= 0x39)
  {
    sb = 7;
  }
  else
    if (t0 >= 0x21)
  {
    sb = 6;
  }
  else
    if (t0 >= 0x1b)
  {
    sb = 5;
  }
  else
    if (t0 >= 0x15)
  {
    sb = 4;
  }
  else
    if (t0 >= 0xf)
  {
    sb = 3;
  }
  else
    if (t0 >= 9)
  {
    sb = 2;
  }
  else
    if (t0 >= 3)
  {
    sb = 1;
  }
  cnt = 1;
  step = 0;
  if (sb >= 5)
  {
    cnt = 2;
  }
  if (sb == 9)
  {
    cnt = 1;
  }
  flag = 0;
  if (sb == 7)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 2;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x3000;
  }
  new_var = sb;
  if (new_var == 8)
  {
    s32 bit = (*((unsigned char *) ((c + 0x6000) + 0x2fc))) & 1;
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 x = *pf;
    cnt = bit + 2;
    *pf = x ^ 1;
    flag = bit + 1;
    if (cnt == 2)
    {
      step = 0x3000;
    }
    else
    {
      step = 0x1800;
    }
  }
  if (new_var == 10)
  {
    cnt = 4;
    step = 0x3000;
  }
  if (sb == 11)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 3;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x1800;
  }
  if (sb >= 12)
  {
    cnt = 6;
    step = 0x1800;
  }
  i = 0;
  if (cnt > 0)
  {
    v80000 = 0x80000;
    v4 = 4;
    v200 = 0x200;
    vB8000 = 0xb8000;
    off = 0x4698;
    ang = 0;
    z0 = 0;
    z1 = 0;
    z2 = 0;
    z3 = 0;
    one = 1;
    j0 = 0;
    do
    {
      s32 z = z0;
      j = j0;
      while (1)
      {
        char *row;
        unsigned char *slot = (unsigned char *) ((c + (j << 6)) + off);
        if ((*slot) == 0)
        {
          unsigned char *ptype;
          *slot = (unsigned char) one;
          (c + (j << 6))[0x4697] = (char) z;
          (c + (j << 6))[0x469b] = (char) z;
          (c + (j << 6))[0x469c] = (char) z;
          (c + (j << 6))[0x469d] = (char) z;
          *((s32 *) ((c + (j << 6)) + 0x4660)) = v80000;
          *((s16 *) ((c + (j << 6)) + 0x4690)) = (s16) v4;
          *((s16 *) ((c + (j << 6)) + 0x4692)) = (s16) v200;
          rnd = (u32) RandomIntInternal(&data_0209d4b8);
          ptype = (unsigned char *) ((int) ( ((int) ((c + (j << 6)) + 0x4696))));
          *ptype = (((rnd >> 16) & 0x7fff) << 1) >> 15;
          *((s32 *) ((c + (j << 6)) + 0x4670)) = 0x999;
          *((s32 *) ((c + (j << 6)) + 0x4688)) = z1;
          if (sb == 0)
          {
            *ptype = (unsigned char) one;
            func_ov006_020d66c4(c, z1);
          }
          if (sb <= 1)
          {
            rnd = (u32) RandomIntInternal(&data_0209d4b8);
            *((s16 *) ((((0, c)) + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
          }
          else
            if (flag != 0)
          {
            if (flag == 1)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0x2000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
              func_ov006_020d66c4(c, z2);
            }
            else
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0xa000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
              func_ov006_020d66c4(c, one);
            }
          }
          else
            if (((*((s32 *) ((c + 0x6000) + 0x2d8))) & 1) != 0)
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0x2800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z3;
            func_ov006_020d66c4(c, z3);
          }
          else
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0xa800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0xa000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
            func_ov006_020d66c4(c, one);
          }
          (*((s32 *) ((int) ( ((int) (c + 0x62d8))))))++;
          break;
        }
        j++;
        if (j >= 0x70)
        {
          break;
        }
      }

      ang += step;
      i++;
    }
    while (i < cnt);
  }
  *((u16 *) ((c + 0x6200) + 0xe2)) = data_ov006_0212e2e8[sb];
}
