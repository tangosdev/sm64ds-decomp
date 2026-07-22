extern void func_ov060_02115a84(void* c, void* p);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* clsn);
extern void func_ov060_02111cc0(void* c, int a, int b);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* clsn);
extern int func_ov060_02112350(void* c);

struct D0211ac88 { int a, b; };
extern struct D0211ac88 data_ov060_0211ac88;

void func_ov060_021134ac(void* thiz)
{
    char* c = (char*)thiz;
    *(unsigned char*)(c + 0x422) = 1;
    func_ov060_02115a84(c, c + 0x3fe);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x14c)) {
        if (*(int*)(c + 0x134) == data_ov060_0211ac88.b) {
            func_ov060_02111cc0(c, 5, 0x40000000);
        }
        {
            int* p = (int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF);
            *p = *p >> 1;
        }
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x14c)) {
        if (!_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x14c)) {
            *(int*)(c + 0x98) = 0;
            {
                unsigned char* p = (unsigned char*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF);
                *p = *p + 1;
            }
        }
    }
    func_ov060_02112350(c);
}
