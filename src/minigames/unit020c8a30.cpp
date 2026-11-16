//cpp
/* ov006 sprite pool, 0x020c8a30..0x020c8dd4.
 *
 * Three 0x20-byte Puff records at sPuffs. Jump, Jump2, Trampoline and
 * Trampoline2 reset the pool through Puff_ClearAll; dMgJump3DMario_c and
 * the Trampoline Mario unit claim records through Puff_Spawn.
 * No RTTI or vtable, so the record name is coined and the C entries keep
 * the names their callers use. Draw, Update and Clear act on one record and
 * are its members; the pool entries call them on each of sPuffs.
 *
 * The file defines the pool. mwcc builds sPuffs in this file's static
 * initializer (retail 0x0212f660) with __cxa_vec_ctor and registers the
 * array destructor __arraydtor, which it emits after the initializer.
 * The record's constructor and destructor are inline, so their used bodies
 * come after __arraydtor as on the cartridge (D1 0x020c8dcc, C1 0x020c8dd0).
 */
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
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

    void Draw();
    void Update();
    void Clear();
};

extern "C" {
/* local extern: arm9 C helpers with no shared header. */
int GetGameLanguage(void);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
}

extern void **sPuffSprites[];

int sPuffNextIndex;
void **sPuffXTable;
Puff sPuffs[3];

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

void Puff::Clear() {
    mActive = 0;
}

void Puff::Update() {
    if (mActive == 0)
        return;
    mAngle += 0x200;
    mSpread = FX_MUL(mSpread, mScale);
    mVelY -= 0x40;
    ApproachLinear(mScale, 0x800, 0x10);
    if (ApproachLinear2(mTimer, 0, 1) != 0) {
        if (mIndex != 2)
            mActive = 0;
    } else {
        x += mVelX;
        y += mVelY;
    }
}

void Puff::Draw() {
    void *sprite;
    int sine;
    int offsetX;
    int drawX;
    if (mActive == 0)
        return;
    if (sPuffXTable != 0) {
        sprite = sPuffXTable[mIndex];
    } else {
        int language = GetGameLanguage();
        sprite = sPuffSprites[language][mIndex];
    }
    sine = FX_SinIdx((unsigned short)mAngle >> 4);
    offsetX = FX_MUL(mSpread, sine);
    drawX = x + offsetX;
    Hud_RenderSprite(sprite, drawX >> 12, y >> 12, -1, -1);
}

void Puff_ClearAll(int a0, int a1) {
    int i;
    for (i = 0; i < 3; i++) {
        sPuffs[i].Clear();
    }
    sPuffNextIndex = a0;
    sPuffXTable = (void **)a1;
}

void Puff_UpdateAll(void) {
    int i;
    for (i = 0; i < 3; i++) {
        sPuffs[i].Update();
    }
}

void Puff_DrawAll(void) {
    int i;
    for (i = 2; i >= 0; i--) {
        sPuffs[i].Draw();
    }
}

inline Puff::~Puff() {}

inline Puff::Puff() {}
