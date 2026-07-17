#pragma opt_strength_reduction off

typedef unsigned short u16;

extern int _ZN5Model8LoadFileER13SharedFilePtr(void *p);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void *p);
extern void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(int bmd, void *bma);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char *t, int f, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *t, int f, int a, int b, unsigned u);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(char *t, void *f, int a, int b, unsigned u);
extern void _ZN5Model12SetPolygonIDEi(char *t, int id);

extern int data_ov006_02141e94;
extern int data_ov006_02141e6c;
extern int data_ov006_0213c7f4;

void func_ov006_020e7954(char *o)
{
    int bca;
    int m1 = -1;
    int z1 = 0;
    int z2 = 0;
    int bmd;
    int i;
    char *w;
    char *w2;
    *(int *)(o + 0x174) = 0x800;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov006_02141e94);
    bca = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e6c);
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(bmd, &data_ov006_0213c7f4);
    w = o;
    w2 = o + 0x12c;
    for (i = 0; i < 3; i++) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(w, bmd, 1, m1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(w, bca, 0x40000000, *(int *)(o + 0x174), z1);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(w2, &data_ov006_0213c7f4, 0x40000000, *(int *)(o + 0x174), z2);
        _ZN5Model12SetPolygonIDEi(w, (i + 1) & 0xff);
        *(int *)(o + i * 4 + 0x168) = 0;
        w += 0x64;
        w2 += 0x14;
    }
    *(u16 *)(o + 0x178) = 0;
    *(u16 *)(o + 0x17a) = 0;
}
