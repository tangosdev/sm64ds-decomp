//cpp
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Camera_State { void *a, *b; };

struct Camera {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual int v9();
    int InitResources();
};

extern "C" {
extern u8 NUM_VIEWS;
extern u8 CURR_PLAYER_ID;
extern void *PLAYER_ARR[];

struct ViewObj { u8 type; u8 p1; s16 x; s16 y; s16 z; };
extern struct ViewObj *GetViewObj(int idx);
extern void *_Znwj(u32 sz);
extern void ChangeArea(int areaID);
extern void func_0200cf40(Camera *thiz);
extern int StartWithFarCamera();
extern void func_0200d0ac(Camera *thiz, u8 id);
}

extern struct Camera_State data_0209b008[];
extern int _ZN6Camera11ChangeStateEPNS_5StateE(Camera *thiz, Camera_State *s);

int Camera::InitResources()
{
    Camera *thiz = this;
    u32 i;
    for (i = 0; i < NUM_VIEWS; i = (u8)(i + 1)) {
        struct ViewObj *v = GetViewObj(i);
        if (v->type == 7) {
            s32 *p;
            s16 x, y, z;
            *(void**)((char*)thiz+0x148) = _Znwj(0xc);
            x = v->x; z = v->z; y = v->y;
            p = *(s32**)((char*)thiz+0x148);
            p[0] = x << 0xc;
            p[1] = y << 0xc;
            p[2] = z << 0xc;
        }
    }
    *(int*)((char*)thiz+0x168) = 0;
    *(void**)((char*)thiz+0x110) = PLAYER_ARR[CURR_PLAYER_ID];
    ChangeArea(*(signed char*)(*(char**)((char*)thiz+0x110)+0xcc));
    *(int*)((char*)thiz+0x114) = 0;
    *(int*)((char*)thiz+0x118) = 0;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, &data_0209b008[0]);
    func_0200cf40(thiz);
    if (StartWithFarCamera() != 0)
        func_0200d0ac(thiz, CURR_PLAYER_ID);
    return this->v9();
}
