#include "types.h"

// dScMgPachinko_c (Bob-omb Squad, scene 368): the hit reaction for bob-omb slot i.
//
// Runs at the tail of the in-flight states (func_ov006_020fd17c, func_ov006_020fd2d8
// and func_ov006_020fd894), after func_ov006_020fdaf0 has done the proximity scan.
// If at least one of the four goal holes (the 0xc-stride records at base+0x5bcc) is
// live and the bob-omb has not left the top of the board, it is re-aimed: a random
// live hole is chosen, cstd::atan2 gives the heading to it, and the velocity is
// rebuilt off the sin/cos table at 0.9x the current speed. The slot is then stamped
// with state 4 plus the index of the hole it is now homing on, and the hit sound
// 0x186 is played, pitched by the round counter.
//
// Per-slot record, stride 0x38 from base+0x4660:
//   +0x00 0x4660 s32 x         +0x04 0x4664 s32 y
//   +0x08 0x4668 s32 velX      +0x0c 0x466c s32 velY
//   +0x26 0x4686 u16 heading   +0x2f 0x468f u8  state
//   +0x34 0x4694 u8  homing target      +0x36 0x4696 u8 round counter
// Goal-hole record, stride 0xc from base+0x5bcc:
//   +0x00 0x5bcc s32 x   +0x04 0x5bd0 s32 y   +0x08 0x5bd4 u8 live

extern s16 data_02082214[];
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern int _ZN4cstd4sqrtEy(u64 x);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
#pragma opt_common_subs off
void func_ov006_020fcb4c(char *base, int i)
{
    int idx; int n; int factor; int dist; int *pY; int *pVx; int *pVy; char *p; char *angBase; s16 angle;
    int count=0,j; char *q;
    /* with every hole closed the retry loop below would never terminate */
    for(j=0,q=base;j<4;j++,q+=0xc){ if(*(u8*)(q+0x5000+0xbd4)!=0){count++;break;} }
    if(!count) return;
    n = i*0x38;
    { char *yBase=base+0x4664; int y=*(int*)(yBase+n); pY=(int*)(yBase+n); if((y>>12)<0) return; }
    {
        /* declaration order here is load bearing: it pins the r4/r5 split and the
           order the two field offsets enter the literal pool */
        char *vxBase = base + 0x4668;
        char *vyBase = base + 0x466c;
        int a = *(int*)(vxBase + n);
        s64 distSq = (s64)a * a;
        int b = *(int*)(vyBase + n);
        pVx = (int*)(vxBase + n);
        distSq += (s64)b * b;
        pVy = (int*)(vyBase + n);
        dist = (int)_ZN4cstd4sqrtEy((u64)distSq);
    }
    factor = dist * 9 / 10;
    /* pick a random live hole to home on */
    idx = ((u32)RandomIntInternal(&data_0209d4b8)>>16 & 0x7fff)*4>>15;
    for(;;){
        int t = idx * 0xc;
        p = base + t;
        p = p + 0x5000;
        if(*(u8*)(p+0xbd4)!=0) break;
        idx = ((u32)RandomIntInternal(&data_0209d4b8)>>16 & 0x7fff)*4>>15;
    }
    {
        char *xBase = base + 0x4660;
        int holeY = *(int*)(p + 0xbd0);
        int yval = *pY;
        int holeX = *(int*)(p + 0xbcc);
        int entX = *(int*)(xBase + n);
        angBase = base + 0x4686;
        angle = _ZN4cstd5atan2E5Fix12IiES1_((holeY-yval)>>12, (holeX-entX)>>12);
        *(u16*)(angBase + n) = (u16)angle;
        u16 angU = *(u16*)(angBase + n);
        int angIdx = (angU >> 4) << 1;
        {
            s16 cosv = data_02082214[angIdx + 1];
            *pVx = (int)(((s64)cosv * factor + 0x800) >> 12);
        }
        {
            u16 angU2 = *(u16*)(angBase + n);
            s16 sinv = data_02082214[(angU2 >> 4) << 1];
            *pVy = (int)(((s64)sinv * factor + 0x800) >> 12);
        }
        int round = *(u8*)(base + n + 0x4000 + 0x696);
        int pitch = (round * 0x180) / 40;
        int handle = func_020126e8(*(int*)(xBase + n));
        func_020126ac(0x186, 6, 0, pitch - 0x80, handle);
        *(u8*)(base + n + 0x4000 + 0x68f) = 4;
        *(u8*)(base + n + 0x4000 + 0x694) = (u8)idx;
    }
}
