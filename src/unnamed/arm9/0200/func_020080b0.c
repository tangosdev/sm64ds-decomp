struct dActor_c;
extern struct dActor_c *func_0200e55c(unsigned int ownerID);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
} CameraView;

int func_020080b0(char *c)
{
    CameraView *cam = (CameraView *)c;
    struct dActor_c *obj = func_0200e55c(0x13);
    char *base = (char *)((char *)obj + 0x5c);

    int z = *(int *)(base + 8);
    /* Camera word 0x170 stays raw: here a height above the actor, elsewhere
       (func_020071e0) a speed, so it has no one role. */
    int y = *(int *)(base + 4) + *(int *)(c + 0x170);
    int x = *(int *)(base + 0);

    cam->lookAt.x = x;
    cam->lookAt.y = y;
    cam->lookAt.z = z;
    return 1;
}
