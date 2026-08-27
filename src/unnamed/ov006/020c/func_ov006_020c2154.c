extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int b, int c);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int b, int c, int scale, unsigned short e);
extern void _ZN5Model12HideMaterialEii(void *self, int a, int b);

void func_ov006_020c2154(char *c)
{
    void *f;
    f = _ZN5Model8LoadFileER13SharedFilePtr(c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x10, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 8);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x10, *(void **)(c + 0xc), 0, 0, 0x800, 0);
    _ZN5Model12HideMaterialEii(c + 0x10, 7, 0);
    *(int *)(c + 0x8c) = 0xd000;
    *(int *)(c + 0x90) = 0;
    *(int *)(c + 0x94) = 0;
    *(short *)(c + 0x98) = 0;
    *(short *)(c + 0x9a) = -0x1400;
    *(short *)(c + 0x9c) = 0;
}
