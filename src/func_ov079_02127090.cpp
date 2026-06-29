//cpp
struct SharedFilePtr { int x; };
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mtx, int fix, short s, void *clps);
extern int IsStarCollectedInCurLevel(int a);
extern struct SharedFilePtr data_ov079_02128300;
extern struct SharedFilePtr data_ov079_021282f0;
extern int data_ov079_02127f64[];
extern signed char data_0209f2f8;
extern int data_0209caa0[];
extern unsigned char data_0209f220;
int func_ov079_02127090(void *self);
}

int func_ov079_02127090(void *self)
{
    char *c = (char*)self;
    void *m;
    signed char st;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128300);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_021282f0);
    m = _ZN5Model8LoadFileER13SharedFilePtr(*(struct SharedFilePtr*)data_ov079_02127f64[0]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    m = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(struct SharedFilePtr*)data_ov079_02127f64[1]);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, m, c + 0x2ec, 0x199, *(short*)(c + 0x8e),
        (void*)data_ov079_02127f64[2]);

    *(int*)(c + 0x320) = 0;

    st = data_0209f2f8;
    if (st == 0x12) {
        if (data_0209caa0[1] & 0x204) return 0;
    }
    if (st == 7) {
        if (data_0209f220 != 1) {
            if (IsStarCollectedInCurLevel(1) != 0) goto ret1;
        }
        if (*(int*)(c + 0x60) >= 0xdac000) return 0;
    }
ret1:
    return 1;
}
