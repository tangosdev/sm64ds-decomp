//cpp
/* ov006 sprite pool, 0x020c8a30..0x020c8dd4.
 *
 * Three 0x20-byte records at sPuffs. Jump, Jump2,
 * Trampoline and Trampoline2 reset the pool through 0x020c8a9c.
 * No RTTI or vtable, so the functions keep their address names.
 * The empty constructor and destructor are what the sinit at
 * 0x0212f660 passes to __cxa_vec_ctor.
 *
 * leftovers:
 * - 020c8ba8 and 020c8af4 still address the record as bytes. The
 *   ((int)c + offset) form is the one those bodies matched.
 * - 020c8c6c clears the active flag through a short* at index 12.
 * - ApproachLinear and ApproachLinear2 stay local externs. They
 *   take the value by reference.
 * - The pool, the counter and the sprite table are overlay data,
 *   not this TU's.
 */
#include "nitro/fx/fx.h"

#include "nitro/fx/fx.h"

typedef long long s64;

struct Ent_ov006 {
    int x;
    int y;
    int mVelX;
    int mVelY;
    int mSpread;
    int mScale;
    short mActive;
    short mIndex;
    short mAngle;
    short mTimer;
};

extern int ApproachLinear(int&, int, int);
extern int ApproachLinear2(short&, short, short);

extern "C" {
int GetGameLanguage(void);
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
typedef void (*dtor_t)(void *);
void __cxa_vec_cleanup(void *block, unsigned int n, unsigned int size, dtor_t dtor);
extern short data_02082214[];
extern void **sPuffSprites[];
extern int sPuffNextIndex;
extern void **sPuffXTable;
extern struct Ent_ov006 sPuffs[3];
extern int sPuffPoolLast[];

void Puff_Draw(char *c);
void Puff_Update(char *c);
void Puff_Clear(short *p);
void Puff_ElemDtor(void);

// @symbol Puff_Nothing
void Puff_Nothing(void)
{
}

// @symbol Puff_ElemDtor
void Puff_ElemDtor(void)
{
}

// @symbol Puff_CleanupAll
void Puff_CleanupAll(void)
{
    __cxa_vec_cleanup(sPuffs, 3, 0x20, (dtor_t)Puff_ElemDtor);
}

// @symbol Puff_Spawn
void Puff_Spawn(int x, int y)
{
    int i;
    int v;

    if (sPuffNextIndex >= 3) {
        return;
    }
    for (i = 0; i < 3; i++) {
        if (sPuffs[i].mActive != 0) {
            continue;
        }
        sPuffs[i].mIndex = sPuffNextIndex;
        sPuffs[i].x = x << 12;
        v = sPuffs[i].x;
        if (v < 0x20000) {
            v = 0x20000;
        } else if (v > 0xE0000) {
            v = 0xE0000;
        }
        sPuffs[i].x = v;
        sPuffs[i].y = y << 12;
        sPuffs[i].mVelX = 0;
        sPuffs[i].mVelY = -0x400;
        sPuffs[i].mActive = 1;
        sPuffs[i].mAngle = 0;
        sPuffs[i].mScale = 0x1000;
        sPuffs[i].mSpread = 0x30000;
        sPuffs[i].mTimer = 90;
        sPuffNextIndex++;
        return;
    }
}

// @symbol Puff_Clear
void Puff_Clear(short *p)
{
    p[12] = 0;
}

// @symbol Puff_Update
void Puff_Update(char* c)
{
    if (*(short*)(c+0x18) == 0) return;
    *(short*)(((int)c + 0x1c)) += 0x200;
    *(int *)(c + 0x10) = FX_MUL(*(int *)(c + 0x10), *(int *)(c + 0x14));
    *(int*)(((int)c + 0xc)) -= 0x40;
    ApproachLinear(*(int*)(c+0x14), 0x800, 0x10);
    if (ApproachLinear2(*(short*)(c+0x1e), 0, 1) != 0) {
        if (*(short*)(c+0x1a) != 2)
            *(short*)(c+0x18) = 0;
    } else {
        *(int*)(c) += *(int*)(c+0x8);
        *(int*)(((int)c + 0x4)) += *(int*)(c+0xc);
    }
}

// @symbol Puff_Draw
void Puff_Draw(char *c)
{
    void *x;
    int t;
    int prod;
    int pos;
    if (*(short*)(c + 0x18) == 0) return;
    if (sPuffXTable != 0) {
        x = sPuffXTable[*(short *)(c + 0x1a)];
    } else {
        int idx2 = GetGameLanguage();
        x = sPuffSprites[idx2][*(short *)(c + 0x1a)];
    }
    t = data_02082214[(*(unsigned short*)(c + 0x1c) >> 4) * 2];
    prod = FX_MUL(*(int *)(c + 0x10), t);
    pos = *(int*)c + prod;
    Hud_RenderSprite(x, pos >> 12, *(int*)(c + 4) >> 12, -1, -1);
}

// @symbol Puff_ClearAll
void Puff_ClearAll(int a0,int a1){
  int i;
  short *p = (short *)sPuffs;
  for(i=0;i<3;i++){
    Puff_Clear(p);
    p=(short*)((char*)p+0x20);
  }
  sPuffNextIndex = a0;
  sPuffXTable = (void **)a1;
}

// @symbol Puff_UpdateAll
void Puff_UpdateAll(void){
  int i = 0;
  char *p = (char *)sPuffs;
  for(;;){
    Puff_Update(p);
    i++;
    p += 0x20;
    if(i >= 3) break;
  }
}

// @symbol Puff_DrawAll
void Puff_DrawAll(void){
  int i;
  char *p = (char *)sPuffPoolLast;
  for(i=2;i>=0;i--){
    Puff_Draw(p);
    p-=0x20;
  }
}
}
