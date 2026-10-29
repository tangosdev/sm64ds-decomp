typedef short s16;
extern void func_ov007_020bc3dc(void* self, int b, int c);

/* The 0x1c-byte animation player func_ov007_020c92ac allocates; names as in
 * func_ov007_020bc7dc, only the slot this file reads. The word padding keeps
 * the whole-record copy word-aligned. */
typedef struct Ov007AnimPlayer {
    int pad_00[3];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[0xe];
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc6d4 and func_ov007_020bc7dc, only the slots this file
 * touches. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18 */
    char pad_1c[0x6c];
    Ov007AnimPlayer *blendAnim; /* 0x88: the cross-fade's source pose */
    s16 blendFrames;            /* 0x8c: frames left of the cross-fade */
    s16 blendFrameCount;        /* 0x8e: the cross-fade's length */
} Ov007Model;

typedef char Ov007AnimPlayer_size_must_be_0x1c[sizeof(Ov007AnimPlayer) == 0x1c ? 1 : -1];
typedef char Ov007Model_size_must_be_0x90[sizeof(Ov007Model) == 0x90 ? 1 : -1];

/* Starts bone animation b: when one is already playing, snapshots its player
 * into blendAnim and cross-fades out of it over d frames. */
void func_ov007_020bc374(char* self, int b, int c, int d) {
    Ov007Model *m = (Ov007Model *)self;
    Ov007AnimPlayer *src = m->boneAnim;
    if (src->anim != -1) {
        *m->blendAnim = *src;
        m->blendFrames = (s16)d;
        m->blendFrameCount = m->blendFrames;
    }
    func_ov007_020bc3dc(self, b, c);
}
