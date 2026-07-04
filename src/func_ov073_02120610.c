    extern void BlendModelAnim_SetAnim(void *self, void *bca, int a, int b, int fix, unsigned short t);
    extern void ApproachLinear(short *p, int target, int step);
    extern short Vec3_HorzAngle(void *a, void *b);
    extern short Vec3_VertAngle(void *a, void *b);
    extern void Matrix4x3_FromRotationY(void *m, int angle);
    extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
    extern void MulVec3Mat4x3(void *in, void *m, void *out);
    extern int WithMeshClsn_IsOnGround(void *self);
    extern void func_ov073_0211f2c0(void *self, int v);
    extern int func_02012694(int a, void *pos);
    extern void func_ov073_0212157c(void *c, void *p);

    extern void **data_ov073_021232a8;
    extern int data_020a0e68;
    extern void *data_ov073_02123320;

    int func_ov073_02120610(void *thiz)
    {
        unsigned char *c = (unsigned char *)thiz;
        int *src;
        int v[3];

        if (*(unsigned short *)(c + 0x100) == 0)
            goto mainblock;

        if (*(unsigned short *)(c + 0x100) == 1) {
            *(int *)(c + 0xa8) = 0x5a000;
            BlendModelAnim_SetAnim(c + 0x30c, (&data_ov073_021232a8)[1], 4, 0, 0x1000, 0);
            *(int *)(c + 0x368) = 0x2000;
        }
        return 1;

    mainblock:
        ApproachLinear((short *)(c + 0x8c), 0, 0x2000);
        if (*(unsigned char *)(c + 0x4cb) == 2) {
            src = (int *)(c + 0x448 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        } else {
            src = (int *)(c + 0x3e8 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        }
        {
            short hz = Vec3_HorzAngle(c + 0x5c, v);
            short vt = Vec3_VertAngle(c + 0x5c, v);
            int in[3];
            int out[3];
            in[2] = 0;
            in[0] = 0; in[1] = 0; out[0] = 0; out[1] = 0; out[2] = 0;
            in[2] = 0x28000;
            Matrix4x3_FromRotationY(&data_020a0e68, hz);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
            MulVec3Mat4x3(in, &data_020a0e68, out);
            *(int *)(c + 0xa4) = out[0];
            if (*(int *)(c + 0x4b4) != 0 || *(int *)(c + 0xa8) < 0) {
                *(int *)(c + 0xa8) = out[1];
                *(int *)(c + 0x4b4) = 1;
            }
            *(int *)(c + 0xac) = out[2];
        }
        if (WithMeshClsn_IsOnGround(c + 0x150) != 0) {
            *(unsigned char *)(c + 0x4c5) = *(unsigned char *)(c + 0x4c4);
            *(int *)(c + 0x98) = 0;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            func_ov073_0211f2c0(c, 0x7d0000);
            func_02012694(0x16d, c + 0x74);
            *(int *)(c + 0x4b8) = 1;
            func_ov073_0212157c(c, &data_ov073_02123320);
        }
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        return 1;
    }
