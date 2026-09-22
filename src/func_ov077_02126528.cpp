//cpp
struct Vector3 { int x, y, z; Vector3() {} };
extern "C" {
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *thiz, const Vector3 &v);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
int _ZN6Player12GetHurtStateEv(void *p);
int _ZN6Player15IsCollectingCapEv(void *p);
void func_ov077_02126d5c(void *c, void *p);
extern int data_ov077_02127a5c[3];
extern int data_ov077_02127cd8;
}

extern "C" void func_ov077_02126528(char *c)
{
    Vector3 v;
    char *a;
    int t;
    v.x = data_ov077_02127a5c[0];
    v.y = data_ov077_02127a5c[1];
    v.z = data_ov077_02127a5c[2];
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x144, v);
    if (*(int *)(c + 0x168) == 0) return;
    a = (char *)_ZN8dActor_c10FindWithIDEj(*(int *)(c + 0x168));
    if (a == 0) return;
    t = (*(unsigned short *)(a + 0xc) == 0xbf);
    if (t == 0) return;
    if (*(unsigned char *)(a + 0x6f9) == 1) return;
    if (*(unsigned char *)(a + 0x703) == 1) return;
    t = (*(unsigned char *)(a + 0x709) != 0);
    if (t == 1) return;
    if (_ZN6Player12GetHurtStateEv(a) >= 0) return;
    if (_ZN6Player15IsCollectingCapEv(a) != 0) return;
    *(int *)(c + 0x400) = (int)a;
    func_ov077_02126d5c(c, &data_ov077_02127cd8);
}
