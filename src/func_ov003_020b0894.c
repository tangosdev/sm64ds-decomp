// NONMATCHING: different op / idiom (div=55). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct VT { void *v0, *v1, *v2, *v3, *v4; int (*m_14)(void *); };
struct Obj { struct VT *vt; };

extern struct Obj *SCENE_FADER;

extern void func_ov003_020b060c(void *self);
extern int IsButtonInputValid(void);
extern void func_ov003_020b0730(void *self, int flag);
extern void func_02012790(int se);
extern void func_0202ae74(void);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned int, unsigned int, unsigned short);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);

extern unsigned char data_020a0e40;
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int GAME_SPEED_RELATED;

int func_ov003_020b0894(unsigned char *self)
{
    struct Obj *o = SCENE_FADER;

    if (o->vt->m_14(o) == 0)
        goto end;

    func_ov003_020b060c(self);

    if (self[0x94] < 2)
        return 1;

    switch (self[0x90]) {
        case 0: goto state0;
        case 1: goto state1;
        default: goto end;
    }

state0:
        {
            unsigned char idx = data_020a0e40;
            int b = 0;
            if (TOUCH_INPUT_ARR[idx * 4] != 0)
                b = (data_020a0de9[idx * 4] != 0);
            if (!b) {
                if (!IsButtonInputValid())
                    goto end;
            }
        }
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x38) < 0x20)
                    goto left;
            }
        }
        if (self[0x91] != 0)
            goto right_check;
        if (!IsButtonInputValid())
            goto right_check;
    left:
        if (self[0x91] == 0)
            self[0x93] = (unsigned char)(GAME_SPEED_RELATED << 3);
        self[0x91] = 0;
        func_ov003_020b0730(self, 0);
        self[0x92] = (unsigned char)(GAME_SPEED_RELATED << 4);
        func_02012790(0x9a);
        self[0x90] = 1;
        goto end;

    right_check:
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x68) < 0x20)
                    goto right;
            }
        }
        if (self[0x91] != 1)
            goto end;
        if (!IsButtonInputValid())
            goto end;
    right:
        if (self[0x91] == 1)
            self[0x93] = (unsigned char)(GAME_SPEED_RELATED << 3);
        self[0x91] = 1;
        func_ov003_020b0730(self, 0);
        self[0x92] = (unsigned char)(GAME_SPEED_RELATED << 4);
        func_02012790(0x9b);
        self[0x90] = 1;
        goto end;

state1:
        if (self[0x93] != 0) {
            self[0x93] -= GAME_SPEED_RELATED;
            if (self[0x93] == 0)
                func_ov003_020b0730(self, 0);
        }
        if (self[0x92] != 0) {
            self[0x92] -= GAME_SPEED_RELATED;
            if (self[0x92] == 0) {
                if (self[0x91] == 0)
                    func_0202ae74();
                else
                    _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            }
        }

end:
    return 1;
}
