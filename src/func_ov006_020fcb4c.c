// @symbol func_ov006_020fcb4c
/* recovered: an ov006 minigame entity re-aims entity `i` at a randomly chosen
 * live player. Bails out if no player slot is live, or if the entity has sunk
 * below y=0. Takes the length of the entity's current velocity vector, scales
 * it to 9/10, picks a random live player slot, and rewrites the velocity as
 * that speed pointed along atan2 towards the chosen player, then fires a sound
 * whose pan comes from the entity's x. Slot state at +0x4000+0x68f/0x694.
 *
 * The two velocity-component base offsets must be introduced in the order
 * below: the constant pool emits 0x4668 before 0x466c, and the declaration
 * order is what fixes it (notes/pret-idioms.md, declaration order controls
 * literal-pool order as well as regalloc). */
#include "types.h"
extern s16 data_02082214[];
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern s64 _ZN4cstd4sqrtEy(u64 x);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
#pragma opt_common_subs off
void func_ov006_020fcb4c(char *base, int i)
{
    int idx; int n; int factor; int dist; int *pY; int *pVx; int *pVy; char *p; char *angBase; s16 angle;
    int count=0,j; char *q;
    for(j=0,q=base;j<4;j++,q+=0xc){ if(*(u8*)(q+0x5000+0xbd4)!=0){count++;break;} }
    if(!count) return;
    n = i*0x38;
    { char *yBase=base+0x4664; int y=*(int*)(yBase+n); pY=(int*)(yBase+n); if((y>>12)<0) return; }
    {
        char *vyBase = base + 0x4668;
        char *vxBase = base + 0x466c;
        int a = *(int*)(vyBase + n);
        s64 distSq = (s64)a * a;
        int b = *(int*)(vxBase + n);
        pVx = (int*)(vyBase + n);
        distSq += (s64)b * b;
        pVy = (int*)(vxBase + n);
        dist = (int)_ZN4cstd4sqrtEy((u64)distSq);
    }
    factor = dist * 9 / 10;
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
        int playerY = *(int*)(p + 0xbd0);
        int yval = *pY;
        int playerX = *(int*)(p + 0xbcc);
        int entX = *(int*)(xBase + n);
        angBase = base + 0x4686;
        angle = _ZN4cstd5atan2E5Fix12IiES1_((playerY-yval)>>12, (playerX-entX)>>12);
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
        int fieldByte = *(u8*)(base + n + 0x4000 + 0x696);
        int div40 = (fieldByte * 0x180) / 40;
        int e8ret = func_020126e8(*(int*)(xBase + n));
        func_020126ac(0x186, 6, 0, div40 - 0x80, e8ret);
        *(u8*)(base + n + 0x4000 + 0x68f) = 4;
        *(u8*)(base + n + 0x4000 + 0x694) = (u8)idx;
    }
}
