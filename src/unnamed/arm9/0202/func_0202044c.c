/* One entry of an OamAnimation's frame table: the OamAttr list drawn for the
 * frame (copied into OamAnimation::tile) and how many ticks it is shown
 * (copied into frameDuration). */
typedef struct OamAnimFrame {
    int tile;     /* 0x00 */
    int duration; /* 0x04 */
} OamAnimFrame;

/* C view of include/OamAnimation.h's OamAnimation (a C++ class this C file
 * cannot include), with the header's names; only the fields used here. */
typedef struct OamAnimation {
    const OamAnimFrame *frames; /* 0x00 */
    int tile;                   /* 0x04 */
    unsigned char pad_08[8];
    int frameCount;    /* 0x10 */
    int frame;         /* 0x14 */
    int frameDuration; /* 0x18 */
    int elapsed;       /* 0x1c */
} OamAnimation;

/* Show frame idx: load its attribute list and duration and restart its
 * tick count. Returns 0 (and changes nothing) past the last frame. */
int func_0202044c(char *this, int idx) {
    OamAnimation *anim = (OamAnimation *)this;

    if (idx >= anim->frameCount)
        return 0;
    anim->frame = idx;
    anim->tile = anim->frames[idx].tile;
    anim->frameDuration = anim->frames[idx].duration;
    anim->elapsed = 0;
    return 1;
}
