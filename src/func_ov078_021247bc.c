typedef struct { int x, y, z; } Vec3;

extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern void ApproachAngle(void *p, int target, int a, int b, int cc);
extern int _ZN9Animation8FinishedEv(void *thiz);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void MulVec3Mat4x3(void *src, void *m, void *dst);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern void func_ov078_02125c48(void *c, void *p);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *thiz, void *f, int a, int b, int fx, unsigned short t);
extern void func_02012694(int a, void *p);
extern void func_ov078_02125c24(void *c, int a);
extern void func_0200fa8c(void *c, int a);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *thiz, void *actor, unsigned int a, const void *pos, unsigned int b, unsigned int cc);
extern void _ZN6Camera9SetFlag_3Ev(void *thiz);

extern char data_020a0e68;
extern char data_ov078_021270fc;
extern char data_ov078_02126f08;
extern char data_0209f318;
extern char data_ov078_021270dc;

int func_ov078_021247bc(void *thiz)
{
    char *c = (char *)thiz;
    short horz;
    short vert;
    Vec3 vA;
    Vec3 vB;
    Vec3 vC;
    Vec3 D;
    Vec3 E;

    if (*(unsigned char *)(c + 0x499) != 2) {
        horz = Vec3_HorzAngle(c + 0x5c, c + 0x4d4);
        vert = Vec3_VertAngle(c + 0x5c, c + 0x4d4);
        Vec3_Dist(c + 0x5c, c + 0x4d4);
        vA.x = 0;
        vA.y = 0;
        vA.z = 0;
        ApproachAngle(c + 0x92, vert, 5, 0x1000, 0x300);
        ApproachAngle(c + 0x94, horz, 5, 0x1000, 0x300);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        if (_ZN9Animation8FinishedEv(c + 0x31c) == 0)
            return 1;
        vB.x = 0;
        vB.y = 0x3c000;
        vB.z = 0x28000;
        vC.x = 0;
        vC.y = 0;
        vC.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0x92));
        MulVec3Mat4x3(&vB, &data_020a0e68, &vC);
        *(int *)(c + 0xa4) = vC.x;
        if (*(unsigned char *)(c + 0x499) == 0) {
            *(int *)(c + 0xa8) = vC.y;
            if (*(int *)(c + 0xa8) > 0x3c000)
                *(int *)(c + 0xa8) = 0x3c000;
            else if (*(int *)(c + 0xa8) < 0x1e000)
                *(int *)(c + 0xa8) = 0x1e000;
        }
        *(int *)(c + 0xac) = vC.z;
        *(int *)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFF) += *(int *)(c + 0x9c);
        vA.x = *(int *)(c + 0x5c);
        vA.y = *(int *)(c + 0x60);
        vA.z = *(int *)(c + 0x64);
        vA.y = *(int *)(c + 0x4d8);
        if (Vec3_Dist(&vA, c + 0x4d4) > 0x640000)
            return 1;
        *(unsigned char *)(c + 0x499) = 1;
    }

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) != 0) {
        void *pl = _ZN5Actor13ClosestPlayerEv(c);
        if (pl != 0) {
            D = *(Vec3 *)((char *)pl + 0x5c);
            if (Vec3_Dist(c + 0x4d4, &D) > 0x640000 ||
                *(int *)(c + 0x4d8) - 0x64000 > D.y) {
                *(int *)(c + 0xa4) = 0;
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0xac) = 0;
                func_ov078_02125c48(c, &data_ov078_021270fc);
                return 1;
            }
        }

        if (*(unsigned char *)(c + 0x499) == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x2cc, *(void **)(&data_ov078_02126f08 + 4), 0, 0x40000000, 0x1000, 0);
            *(int *)(c + 0x9c) = -0x2000;
            *(int *)(c + 0xb0) = 0x10000002;
            func_02012694(0x12c, c + 0x74);
            func_ov078_02125c24(c, 0xfa0000);
            func_0200fa8c(c, 1);
            *(unsigned char *)(c + 0x499) = 2;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            return 1;
        }

        if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
            char *pl2 = *(char **)(c + 0x430);
            unsigned short m;
            E.x = *(int *)(c + 0x5c);
            E.y = *(int *)(c + 0x60);
            E.z = *(int *)(c + 0x64);
            E.y += 0xc8000;
            m = *(unsigned short *)(pl2 + 0x6ce) & 0x800;
            if (m == 0) {
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(pl2, c, 0x94, &E, 0, 0) != 0) {
                    _ZN6Camera9SetFlag_3Ev(*(void **)&data_0209f318);
                    func_02012694(0x12a, c + 0x74);
                    func_ov078_02125c48(c, &data_ov078_021270dc);
                }
            }
        }
    }

    return 1;
}
