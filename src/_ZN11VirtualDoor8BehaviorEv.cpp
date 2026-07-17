//cpp
typedef struct Mtx { int m[12]; } Mtx;
extern "C" {
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void InvMat4x3(void *in, void *out);
extern void func_ov002_020b0a0c(void *c);
extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern int _ZN6Player20IsStateEnteringLevelEv(void *p);
extern int _ZN6Player12Unk_020c9e5cEh(void *p, int a);
extern void StartExitFaderWipe(int a);
extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern void _ZN6Player17SetNoControlStateEhih(void *p, int a, int b, int d);
extern char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *prev);
extern int Vec3_Dist(void *a, void *b);
extern void func_02012790(int id);
extern void _ZN6Camera10LookAtExitER5Actor(void *cam, void *a);

extern unsigned char data_0209f250;
extern char *data_0209f394[];
extern signed char data_02092110;
extern char data_0209f5e8[];
extern char *data_0209f318;
extern Mtx data_020a0e68;

int _ZN11VirtualDoor8BehaviorEv(char *c) {
    int out1[3];
    int out2[3];
    char *player = data_0209f394[data_0209f250];
    if (*(int *)(c + 0x98) != 0) {
        MulVec3Mat4x3(player + 0x5c, c + 0xd4, out1);
        if (out1[2] > *(int *)(c + 0x98) || out1[2] < -0x300000) {
            out1[2] = *(int *)(c + 0x98);
            data_020a0e68 = *(Mtx *)(c + 0xd4);
            InvMat4x3(&data_020a0e68, &data_020a0e68);
            MulVec3Mat4x3(out1, &data_020a0e68, player + 0x5c);
        }
        *(int *)((((int)c) + 0x98) & 0xFFFFFFFFFFFFFFFFLL) -= 0x20000;
        if (*(int *)(c + 0x98) < -0x300000) {
            *(int *)(c + 0x98) = -0x300000;
            if (data_02092110 < 0) {
                func_ov002_020b0a0c(c);
                _ZN5Scene20SetAndStopColorFaderEv();
                *(short *)(data_0209f5e8 + 0xc) = 0x7fff;
            }
        }
    } else {
        if (data_02092110 < 0) {
            if (_ZN6Player20IsStateEnteringLevelEv(player) == 0) {
                MulVec3Mat4x3(player + 0x5c, c + 0xd4, out2);
                {
                    int a = out2[0];
                    if (a < 0)
                        a = -a;
                    if (a <= *(int *)(c + 0x80)) {
                        if (out2[1] >= 0 && out2[1] <= *(int *)(c + 0x84)) {
                            if ((out2[2] ^ *(int *)(c + 0x88)) < 0) {
                                char *cam;
                                *(unsigned short *)(c + 0x90) = 0;
                                if (out2[1] > 0x100000) {
                                    *(unsigned short *)(c + 0x90) = 2;
                                } else if (out2[1] > 0x60000) {
                                    *(unsigned short *)(c + 0x90) = 1;
                                }
                                cam = data_0209f318;
                                if (_ZN6Player12Unk_020c9e5cEh(player, 7)) {
                                    func_ov002_020b0a0c(c);
                                    StartExitFaderWipe(6);
                                    _ZN6Camera9SetFlag_3Ev(cam);
                                } else {
                                    _ZN6Player17SetNoControlStateEhih(player, 6, -1, 0);
                                    if (*(short *)(c + 0x8c) != 0) {
                                        char *o;
                                        func_ov002_020b0a0c(c);
                                        StartExitFaderWipe(5);
                                        _ZN6Camera9SetFlag_3Ev(cam);
                                        o = _ZN5Actor15FindWithActorIDEjPS_(0x12a, 0);
                                        while (o != 0) {
                                            if (Vec3_Dist(o + 0x5c, c + 0x5c) < 0x12c000)
                                                break;
                                            o = _ZN5Actor15FindWithActorIDEjPS_(0x12a, o);
                                        }
                                        if (o != 0) {
                                            func_02012790(0x16);
                                        } else if (data_02092110 == 0xd) {
                                            func_02012790(0x1e);
                                        } else {
                                            func_02012790(0x19);
                                        }
                                    } else {
                                        int t = (signed char)(int)((*(unsigned int *)(c + 8) >> 0x18) &
                                                                   0xFFFFFFFFFFFFFFFFLL);
                                        if (t == 0x1b || t == 0x12) {
                                            func_ov002_020b0a0c(c);
                                            _ZN5Scene20SetAndStopColorFaderEv();
                                            *(short *)(data_0209f5e8 + 0xc) = 0x7fff;
                                        }
                                        *(int *)(c + 0x98) = out2[2];
                                        _ZN6Camera10LookAtExitER5Actor(cam, c);
                                        func_02012790(0x1e);
                                    }
                                }
                            }
                        }
                    }
                }
                *(int *)(c + 0x88) = out2[2];
            }
        }
    }
    return 1;
}
}
