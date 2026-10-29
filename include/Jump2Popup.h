#ifndef JUMP2POPUP_H
#define JUMP2POPUP_H

/* Jump2Popup, a 0x24-byte score popup: dScMgJump2_c::mPopups (0x10 of them)
 * and dScMgTrampoline2_c's 0xa-element array at 0x6ffc. The name is coined;
 * its methods' mangled names (_ZN10Jump2Popup5SpawnEiii and kin) are this
 * tree's. Fields are read back from what src/minigames/d_s_mg_jump2.cpp does
 * with them. The constructor and destructor are empty and out of line in that
 * file (ROM 0x020efc08 and 0x020eed64, one `bx lr` each), which is what both
 * scenes' __cxa_vec_ctor / __cxa_vec_cleanup calls register. */
struct Jump2Popup {
    Jump2Popup();
    ~Jump2Popup();

    struct Jump2Popup *next;
    /* 0x00 -- the live list */
    int x, y;
    /* 0x04 -- 20.12 position */
    int targetX, targetY;
    /* 0x0c */
    int speedX, speedY;
    /* 0x14 */
    short life;
    /* 0x1c -- frames left */
    short number;
    /* 0x1e -- displayed numeric value */
    short active;
    /* 0x20 */

    short _22;

    void Spawn(int, int, int); /* spawn: start x, start y, value */
    void Tick();               /* update */
    void DrawNumber();         /* draw   */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Jump2Popup_size_must_be_0x24[sizeof(Jump2Popup) == 0x24 ? 1 : -1];
#endif

#endif
