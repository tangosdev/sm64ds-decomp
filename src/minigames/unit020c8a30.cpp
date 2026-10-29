//cpp
/* ov006 sprite pool, 0x020c8a30..0x020c8dd4.
 *
 * Three 0x20-byte Puff records at sPuffs. Jump, Jump2, Trampoline and
 * Trampoline2 reset the pool through Puff_ClearAll; dMgJump3DMario_c and
 * the Trampoline Mario unit claim records through Puff_Spawn.
 * No RTTI or vtable, so the record name is coined and the C entries keep
 * the names their callers use.
 *
 * The file defines the pool. mwcc builds sPuffs in this file's static
 * initializer (retail 0x0212f660) with __cxa_vec_ctor and registers the
 * array destructor __arraydtor, which it emits after the initializer.
 * The record's constructor and destructor are inline, so their used bodies
 * come after __arraydtor as on the cartridge (D1 0x020c8dcc, C1 0x020c8dd0).
 */
#include "nitro/fx/fx.h"
#include "math/Approach.h"

/* One puff sprite: position, velocity, a swaying spread that shrinks by
   mScale each frame, and a lifetime. */
struct Puff {
    Puff();
    ~Puff();

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

extern "C" {
/* local extern: arm9 C helpers with no shared header. */
int GetGameLanguage(void);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
}

extern short data_02082214[];
extern void **sPuffSprites[];

int sPuffNextIndex;
void **sPuffXTable;
Puff sPuffs[3];

extern "C" {

// @symbol Puff_Spawn
void Puff_Spawn(int x, int y) {
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
void Puff_Clear(short *p) {
    ((Puff *)p)->mActive = 0;
}

// @symbol Puff_Update
void Puff_Update(char *c) {
    Puff *entry = (Puff *)c;
    if (entry->mActive == 0)
        return;
    entry->mAngle += 0x200;
    entry->mSpread = FX_MUL(entry->mSpread, entry->mScale);
    entry->mVelY -= 0x40;
    ApproachLinear(entry->mScale, 0x800, 0x10);
    if (ApproachLinear2(entry->mTimer, 0, 1) != 0) {
        if (entry->mIndex != 2)
            entry->mActive = 0;
    } else {
        entry->x += entry->mVelX;
        entry->y += entry->mVelY;
    }
}

// @symbol Puff_Draw
void Puff_Draw(char *c) {
    Puff *entry = (Puff *)c;
    void *sprite;
    int sine;
    int offsetX;
    int drawX;
    if (entry->mActive == 0)
        return;
    if (sPuffXTable != 0) {
        sprite = sPuffXTable[entry->mIndex];
    } else {
        int language = GetGameLanguage();
        sprite = sPuffSprites[language][entry->mIndex];
    }
    sine = data_02082214[((unsigned short)entry->mAngle >> 4) * 2];
    offsetX = FX_MUL(entry->mSpread, sine);
    drawX = entry->x + offsetX;
    Hud_RenderSprite(sprite, drawX >> 12, entry->y >> 12, -1, -1);
}

// @symbol Puff_ClearAll
void Puff_ClearAll(int a0, int a1) {
    int i;
    short *p = (short *)sPuffs;
    for (i = 0; i < 3; i++) {
        Puff_Clear(p);
        p = (short *)((char *)p + 0x20);
    }
    sPuffNextIndex = a0;
    sPuffXTable = (void **)a1;
}

// @symbol Puff_UpdateAll
void Puff_UpdateAll(void) {
    int i = 0;
    char *p = (char *)sPuffs;
    for (;;) {
        Puff_Update(p);
        i++;
        p += 0x20;
        if (i >= 3)
            break;
    }
}

// @symbol Puff_DrawAll
void Puff_DrawAll(void) {
    int i;
    char *p = (char *)&sPuffs[2];
    for (i = 2; i >= 0; i--) {
        Puff_Draw(p);
        p -= 0x20;
    }
}
}

// @symbol _ZN4PuffD1Ev
inline Puff::~Puff() {}

// @symbol _ZN4PuffC1Ev
inline Puff::Puff() {}
