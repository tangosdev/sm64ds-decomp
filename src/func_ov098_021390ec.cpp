//cpp
// @symbol func_ov098_021390ec
/* recovered: shared common types */
#include "common.h"
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;


struct ClsnResult { int GetClsnID() const; };
struct SurfaceInfo { void CopyNormalTo(Vector3 &) const; };
struct WithMeshClsn {
    int IsOnWall() const;
    ClsnResult *GetWallResult() const;
};
struct Actor {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void vcall();
    static Actor *FindWithID(u32 id);
    s16 ReflectAngle(int a, int b, s16 c);
};

extern "C" u8 DecIfAbove0_Byte(u8 *p);
extern int func_ov002_020ef228(void *c, int arg);

extern "C" void func_ov098_021390ec(char *cc)
{
    Actor *c = (Actor *)cc;
    if (DecIfAbove0_Byte((u8 *)((char *)c + 0x605)) != 0)
        return;
    if (((WithMeshClsn *)((char *)c + 0x320))->IsOnWall() != 0) {
        ClsnResult *wr = ((WithMeshClsn *)((char *)c + 0x320))->GetWallResult();
        if (wr->GetClsnID() != -1) {
            Actor *a = Actor::FindWithID((u32)wr->GetClsnID());
            if (a != 0) {
                int isF = (*(unsigned short *)((char *)a + 0xc) == 0xf);
                if (isF == 0) {
                    c->vcall();
                    return;
                }
            }
        }
    }
    if (func_ov002_020ef228((char *)c + 0x320, (int)c) != 0) {
        *(u8 *)((char *)c + 0x605) = 3;
        return;
    }
    if (((WithMeshClsn *)((char *)c + 0x320))->IsOnWall() == 0)
        return;
    if (*(int *)((char *)c + 0x98) > 0x14000) {
        c->vcall();
        return;
    }
    Vector3 v;
    ((SurfaceInfo *)((char *)((WithMeshClsn *)((char *)c + 0x320))->GetWallResult() + 4))->CopyNormalTo(v);
    *(s16 *)((char *)c + 0x94) = c->ReflectAngle(v.x, v.z, *(s16 *)((char *)c + 0x94));
}
