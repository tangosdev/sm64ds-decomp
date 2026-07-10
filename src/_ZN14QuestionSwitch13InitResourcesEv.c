extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int b, int c);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(void *self, int flags);
extern void func_ov002_020b50a0(char *c);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *f, void *m, int fix, short sh, void *b);
extern void func_020393c4(int *p, int v);
extern int _ZNK9Animation13GetFrameCountEv(void *self);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void func_ov002_020b503c(char *self);

extern int data_ov002_0210dd60;
extern int data_ov002_0210dd68;
extern int data_ov002_0210dd58;
extern int data_ov002_0210dd50;
extern int data_ov002_0210d8b4;
extern int data_ov002_0210d774;
extern void func_ov002_020b56c4(void);
extern int data_0209caa0[];

int _ZN14QuestionSwitch13InitResourcesEv(char *self)
{
    void *f;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210dd60);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x6b4, f, 1, -1);

    f = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210dd68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x6b4, f, 0, 0x1000, 0);
    _ZN9Animation8SetFlagsEi(self + 0x704, 0x40000000);
    *(int *)(self + 0x710) = 0x1000;
    func_ov002_020b50a0(self);

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_0210dd58);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x324, f, self + 0x2ec, 0x199, *(short *)(self + 0x8e), &data_ov002_0210d8b4);

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_0210dd50);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x4ec, f, self + 0x2ec, 0x199, *(short *)(self + 0x8e), &data_ov002_0210d774);

    func_020393c4((int *)(self + 0x324), (int)&func_ov002_020b56c4);

    if (data_0209caa0[1] & 0x80000000) {
        *(int *)(self + 0x320) = (int)(self + 0x4ec);
        *(int *)(self + 0x70c) = _ZNK9Animation13GetFrameCountEv(self + 0x704) << 12;
        _ZN9Animation7AdvanceEv(self + 0x704);
    } else {
        *(int *)(self + 0x320) = (int)(self + 0x324);
        *(int *)(self + 0x70c) = 0;
    }

    func_ov002_020b503c(self);
    *(char *)(self + 0x718) = 8;
    *(int *)(self + 0x720) = 0;
    return 1;
}
