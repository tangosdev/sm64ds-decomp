//cpp
struct Obj {
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
    virtual int v7();
};
extern "C" {
extern Obj* SCENE_FADER;
extern int KS_FADER;
}
extern "C" void FUN_02029934(void) {
    Obj* o = SCENE_FADER;
    if (o->v7() == 0) {
        KS_FADER = 0;
        *(volatile unsigned short*)0x4000050 = 0;
    }
}
