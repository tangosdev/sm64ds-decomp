//cpp
struct GVT {
    int (*f00)(void*);
    int (*f04)(void*);
    int (*f08)(void*);
    int (*f0c)(void*);
    int (*f10)(void*);
    int (*f14)(void*);
    int (*f18)(void*);
};
struct GObj { struct GVT *vt; };
extern "C" struct GObj *data_0209f5bc;

struct Scene {
    virtual int v00();
    virtual int v04();
    virtual int v08();
    virtual int v0c();
    virtual int v10();
    virtual int v14();
    virtual int v18();
    virtual int v1c();
    virtual int v20();
    virtual int v24();
};
extern "C" Scene *data_0209d4ac;

extern "C" void _ZN5Scene9SetFadersEP15FaderBrightness(Scene *thiz)
{
    if (data_0209f5bc) {
        if (data_0209f5bc->vt->f14(data_0209f5bc)) {
            thiz->v24();
        } else if (data_0209f5bc->vt->f18(data_0209f5bc)) {
            thiz->v20();
        }
    }
    data_0209f5bc = (struct GObj*)thiz;
    data_0209d4ac = thiz;
}
