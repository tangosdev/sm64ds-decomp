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
extern "C" struct GObj *SCENE_FADER;

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
    if (SCENE_FADER) {
        if (SCENE_FADER->vt->f14(SCENE_FADER)) {
            thiz->v24();
        } else if (SCENE_FADER->vt->f18(SCENE_FADER)) {
            thiz->v20();
        }
    }
    SCENE_FADER = (struct GObj*)thiz;
    data_0209d4ac = thiz;
}
