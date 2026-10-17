struct dActor_c;
extern struct dActor_c *func_0200e55c(unsigned int ownerID);
extern int Math_Function_0203b0fc(int a0,int a1,int a2,int a3);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    Vec3 pos;    /* 0x08c */
} CameraView;

int func_020080f0(void *self)
{
    CameraView *cam = (CameraView *)self;
    struct dActor_c *tmp = func_0200e55c(0x13);
    int r4 = (int)tmp + 0x5c;
    int v1 = *(int *)(r4 + 4);
    Math_Function_0203b0fc((int)&cam->pos.y, v1 + 0x23000, 0x51, 0x7fffffff);
    int v2 = *(int *)(r4 + 4);
    Math_Function_0203b0fc((int)&cam->lookAt.y, v2 + 0x7d000, 0x266, 0x7fffffff);
    return 1;
}
