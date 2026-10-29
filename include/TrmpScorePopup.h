#ifndef TRMPSCOREPOPUP_H
#define TRMPSCOREPOPUP_H

#include "types.h"

struct Node;

/* TrmpScorePopup, one floating score of the Trampoline scenes' five-slot pool
 * (dScMgTrampoline_c at 0x5cd0, dScMgTrampoline2_c::mScorePopups). Spawned
 * when a Mario lands: x starts at the right edge and drifts left, y eases
 * toward targetY, and number is the score drawn by Mg_DrawBigNumber. Same 0x24
 * shape as dScMgJump2_c's Jump2Popup. targetX is written and not read by the
 * step. The name is coined; no RTTI names it.
 *
 * The constructor and destructor are the class's own and empty, out of line in
 * src/minigames/d_s_mg_trampoline.cpp (ROM 0x021225a8 and 0x02120938); both
 * scenes hand them to the array helpers. The pool (TrmpFx_*) is free functions
 * over the file-scope live list there. */
struct TrmpScorePopup {
    TrmpScorePopup();
    ~TrmpScorePopup();

    Node *next;  /* +0x00 -- the live list */
    int x;       /* +0x04 -- 20.12 */
    int y;       /* +0x08 */
    int targetX; /* +0x0c */
    int targetY; /* +0x10 */
    int speedX;  /* +0x14 */
    int speedY;  /* +0x18 -- func_0203d630 damps it with speedX (it
                       scales both words); not applied here */
    u16 life;    /* +0x1c */
    u16 number;  /* +0x1e */
    u16 active;  /* +0x20 */

    void Spawn(int, int); /* (y, number): fills a slot and links it live */
    void Update();        /* ticks life, eases toward targetY */
    void Render();        /* draws the number when active */
};

#ifndef SM64DS_PLATFORM_PC
typedef char TrmpScorePopup_size_must_be_0x24[sizeof(TrmpScorePopup) == 0x24 ? 1 : -1];
#endif

#endif
