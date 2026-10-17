typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;

extern void func_02007c9c(const Vector3 *v0, const Vector3 *v1, int *outDist, short *outVertAng, short *outHorzAng);
extern int func_0200e728(unsigned char *p);
extern short ReadUnalignedShort(unsigned char *p);
extern void func_02007c14(Vector3 *res, const Vector3 *trans, Fix12i mag, short ang, short angY);

/* File-local views (the class headers are C++): only the fields this script
 * op touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h (mPos spans its mPosX..mPosZ). */
typedef struct {
    char pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
} PlayerView;

typedef struct {
    char pad_000[0x8c];
    Vector3 pos; /* 0x08c */
    char pad_098[0x78];
    PlayerView *mTargetPlayer; /* 0x110 */
} CameraView;

int func_02007910(void *thiz, unsigned char *p)
{
    CameraView *cam = (CameraView *)thiz;
    Vector3 *v = &cam->mTargetPlayer->mPos;
    int dist;
    short vert;
    short horz;

    func_02007c9c(v, &cam->pos, &dist, &vert, &horz);
    dist = func_0200e728(p) + dist;
    vert = (short)(ReadUnalignedShort(p + 4) + vert);
    horz = (short)(ReadUnalignedShort(p + 6) + horz);
    func_02007c14(&cam->pos, v, dist, vert, horz);
    return 1;
}
