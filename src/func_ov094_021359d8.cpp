//cpp
extern "C" void ModelAnim_SetAnim(void *self, void *bca, int a, int fix, unsigned int b);
extern "C" int Sound_PlayLong(unsigned int a, unsigned int b, unsigned int cc, void *pos, unsigned int d);
extern "C" int func_ov002_020df7f4(void *c);
extern "C" int WithMeshClsn_IsOnWall(void *self);
extern "C" int func_02035638(void *p);
extern "C" int func_ov002_020df7ac(void *thiz);
extern "C" void WithMeshClsn_Unk_0203589c(void *self);
extern "C" void func_ov094_02136188(void *c, void *p);
extern "C" int func_02012694(int a, void *pos);
extern "C" int ApproachAngle(short *p, int target, int a, int b, int c);

extern void **data_ov094_02136ae8;
extern void **data_ov094_02136af8;
extern void *data_ov094_02136b30;

extern "C" int func_ov094_021359d8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    void *rider;

    if (*(unsigned short *)(c + 0x100) == 1) {
        ModelAnim_SetAnim(c + 0x30c, (&data_ov094_02136ae8)[1], 0, 0x1000, 0);
        *(int *)(c + 0x3f0) = 0x1000;
    }

    if (*(unsigned short *)(c + 0x100) == 0) {
        *(int *)(c + 0x3f4) = Sound_PlayLong(*(unsigned int *)(c + 0x3f4), 3, 0x18c, c + 0x74, 0);
    }

    if (*(unsigned short *)(c + 0x100) != 0) {
        if (*(int *)(c + 0x3e8) == 0
            && ((unsigned int)(*(int *)(c + 0x364) << 4) >> 16) <= 2) {
            func_02012694(0x139, c + 0x74);
            *(int *)(c + 0x3e8) = 1;
        } else {
            if (((unsigned int)(*(int *)(c + 0x364) << 4) >> 16) > 2)
                *(int *)(c + 0x3e8) = 0;
        }
    }

    rider = *(void **)(c + 0x3cc);
    if (rider != 0 && func_ov002_020df7f4(rider) == 1) {
        *(unsigned short *)(c + 0x100) = 0xa;
        ModelAnim_SetAnim(c + 0x30c, (&data_ov094_02136af8)[1], 0, 0x1000, 0);
    }

    ApproachAngle((short *)(c + 0x92), 0, 0xa, 0x200, 0x100);

    if (WithMeshClsn_IsOnWall(c + 0x150) != 0 || func_02035638(c + 0x150) != 0) {
        rider = *(void **)(c + 0x3cc);
        if (rider != 0 && func_ov002_020df7ac(rider) != 0) {
            WithMeshClsn_Unk_0203589c(c + 0x150);
            *(int *)(c + 0x3cc) = 0;
            *(int *)(c + 0x3e8) = 0;
            *(unsigned short *)(c + 0x100) = 0;
            func_ov094_02136188(c, &data_ov094_02136b30);
            return 1;
        }
    }

    rider = *(void **)(c + 0x3cc);
    if (rider == 0) {
        goto cleanup;
    }
    if (rider == 0) {
        goto end;
    }
    if (func_ov002_020df7f4(rider) < 0) {
cleanup:
        *(int *)(c + 0x3cc) = 0;
        *(int *)(c + 0x3e8) = 0;
        *(unsigned short *)(c + 0x100) = 0;
        func_ov094_02136188(c, &data_ov094_02136b30);
    }
end:
    return 1;
}
