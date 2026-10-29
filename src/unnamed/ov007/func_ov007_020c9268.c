typedef struct Ov007AnimPlayer {
    /* 0x00: 0 playing; 1 held (func_ov007_020c9110 leaves the frame alone);
     * func_ov007_020c9110 sets 2 when a one-shot ends */
    int state;
    /* 0x04: what func_ov007_020c9110 does at either end: 0 stops (state 2),
     * 1 wraps to the other end, 2 negates speed and plays back */
    int loopMode;
    /* 0x08: bit 0, set by func_ov007_020c9214: func_ov007_020c9110 skips
     * one advance and clears it, so the start frame is shown once */
    int flags;
    short anim; /* 0x0c: index into the owner's animation table; -1 none */
    /* 0x0e: func_ov007_020c9214 moves the old anim here before it installs
     * the new one; no reader */
    short prevAnim;
    unsigned short frame;      /* 0x10: the whole frame func_ov007_020c9110 last reached */
    unsigned short frameCount; /* 0x12 */
    /* 0x14: Q12 frame position; func_ov007_020c9110 adds speed to it and
     * clamps it to 0 .. (frameCount - 1) << 12 */
    int framePos;
    int speed; /* 0x18: Q12 frames per tick; func_ov007_020c9214's callers pass 0x1000 */
} Ov007AnimPlayer;

typedef char Ov007AnimPlayer_size_must_be_0x1c[sizeof(Ov007AnimPlayer) == 0x1c ? 1 : -1];

extern void *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c9268(char *o);
extern int func_ov007_020c3d1c(void);

void *func_ov007_020c92ac(void) {
    void *c = func_ov007_020c3df4(0, 0x1c);
    func_ov007_020c9268(c);
    return c;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
int func_ov007_020c92a0(void) {
    return func_ov007_020c3d1c();
}
#pragma long_calls off

/* The 0x1c-byte animation player func_ov007_020c92ac allocates and this
 * function resets. state, anim, frame and frameCount are named as in
 * func_ov007_020bc02c; the rest from func_ov007_020c9214 (which starts an
 * animation) and func_ov007_020c9110 (which ticks one). */
/* Resets a player: stopped (state 2), one-shot, no animation, at frame 0.
 * The parameter keeps the tree's char * spelling; `p` is the typed view. */
void func_ov007_020c9268(char *o) {
    Ov007AnimPlayer *p = (Ov007AnimPlayer *)o;
    p->state = 2;
    p->loopMode = 0;
    p->flags = 1;
    p->anim = -1;
    p->prevAnim = -1;
    p->frame = 0;
    p->frameCount = 0;
    p->framePos = 0;
    p->speed = 0;
}
