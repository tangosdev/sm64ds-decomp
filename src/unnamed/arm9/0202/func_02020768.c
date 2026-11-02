extern int func_0202044c(char *thiz, int idx);
extern int func_0202043c(char *thiz);

extern int data_0208ee44;

/* C view of include/OamAnimation.h's OamAnimation (a C++ class this C file
 * cannot include), with the header's names; the header's names. */
typedef struct OamAnimation {
    const void *frames; /* 0x00 */
    int tile;           /* 0x04 */
    int scaleX;         /* 0x08 */
    int scaleY;         /* 0x0c */
    int frameCount;     /* 0x10 */
    int frame;         /* 0x14 */
    int frameDuration; /* 0x18 */
    int elapsed;       /* 0x1c */
    short rotation;    /* 0x20 */
    signed char palette;   /* 0x22 */
    signed char priority;  /* 0x23 */
    unsigned char loops;   /* 0x24 */
    unsigned char enabled; /* 0x25 */
} OamAnimation;

/* Advance an enabled animation by data_0208ee44 ticks: once elapsed reaches
 * frameDuration, step to the next frame (func_0202044c); past the last one,
 * restart (func_0202043c) -- forever with loops 0, otherwise until loops
 * counts down to 1, where it stops. */
void func_02020768(char *thiz)
{
    OamAnimation *anim = (OamAnimation *)thiz;

    if (anim->enabled == 0)
        return;

    anim->elapsed += data_0208ee44;

    if (anim->elapsed < anim->frameDuration)
        return;

    anim->elapsed = 0;

    if (func_0202044c(thiz, anim->frame + 1) != 0)
        return;

    if (anim->loops == 0) {
        func_0202043c(thiz);
        return;
    }

    if (anim->loops <= 1) {
        anim->enabled = 0;
        return;
    }

    anim->loops -= 1;
    func_0202043c(thiz);
}
