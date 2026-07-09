struct V3 { int x, y, z; };
struct V3s { short x, y, z; };

extern struct V3 data_020a0ebc;
extern struct V3s data_020a0edc;

extern void _ZN11ShadowModel8CleanAllEv(void);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int b, int c);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int b, int c, int scale, unsigned short e);
extern void _ZN5Model12HideMaterialEii(void *self, int a, int b);
extern void func_ov006_020c0af8(char *c);
extern void func_ov006_020c1764(char *c);
extern void func_ov006_020c092c(char *c);

int func_ov006_020c1a88(char *c)
{
    void *f;
    void *ip;

    _ZN11ShadowModel8CleanAllEv();
    *(void **)(c + 0x260) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1e8);
    *(void **)(c + 0x264) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1f0);
    f = *(void **)(c + 0x260);
    if (f == 0 || _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x1c, f, 1, -1) == 0) {
        return 0;
    }
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x1f8);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x200);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x208);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x210);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x218);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x220);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x228);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x230);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x238);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x240);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x248);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x250);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x258);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x1fc), 0, 0, 0x800, 0);
    func_ov006_020c0af8(c);
    *(int *)(c + 8) = data_020a0ebc.x;
    *(int *)(c + 0xc) = data_020a0ebc.y;
    *(int *)(c + 0x10) = data_020a0ebc.z;
    *(short *)(c + 0x14) = data_020a0edc.x;
    *(short *)(c + 0x16) = data_020a0edc.y;
    *(short *)(c + 0x18) = data_020a0edc.z;
    *(int *)(c + 0x1d4) = 0;
    _ZN5Model12HideMaterialEii(c + 0x1c, 1, 0);
    func_ov006_020c1764(c);
    f = *(void **)(c + 0x264);
    if (f == 0 || _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x8c, f, 1, -1) == 0) {
        return 0;
    }
    ip = *(void **)(c + 0x98);
    *(int *)(((long long)(int)((char *)ip + 0x24)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
    *(int *)(((long long)(int)((char *)ip + 0x114)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
    *(int *)(((long long)(int)((char *)ip + 0xe4)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
    func_ov006_020c092c(c + 0xdc);
    *(short *)(c + 0x1e0) = 0;
    *(short *)(c + 0x1e2) = 1;
    *(short *)(c + 0x1e4) = 0;
    return 1;
}
