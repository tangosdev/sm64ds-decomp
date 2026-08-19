//cpp
extern "C" {
struct Vector3 { int x, y, z; };
struct dActor_c;
struct dBgCh_Lin { char data[0x78]; };

extern void func_020374b8(int *a, int *b);
extern void func_0203aa74(void *thiz, Vector3 *v, Vector3 *res);
extern void _ZN9dBgCh_LinC1Ev(dBgCh_Lin *);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin *, const Vector3 *, const Vector3 *, dActor_c *);
extern void func_02035394(dBgCh_Lin *, void *);
extern int _ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin(void *, dBgCh_Lin *);
extern Vector3 *_ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *, dBgCh_Lin *);
extern void func_0203aa10(void *thiz, const Vector3 *v, Vector3 *res);
extern void _ZN5dBgPiaSERKS_(void *, const void *);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_Lin *);

int _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Gnd(void *self, char *ground)
{
    Vector3 v0, v0xc, v0x18, v0x24, v0x30, v0x3c;
    dBgCh_Lin ray;

    func_020374b8((int *)ground, (int *)&v0xc);
    v0x24 = v0xc;

    int b4c = *(int *)(ground + 0x4c);
    if (*(unsigned char *)(ground + 0x48) != 0) {
        int diff = v0xc.y - *(int *)(ground + 0x44);
        if (diff < b4c) b4c = diff;
    }
    v0x24.y = v0x24.y - b4c;

    func_0203aa74(self, &v0xc, &v0);
    func_0203aa74(self, &v0x24, &v0x18);

    _ZN9dBgCh_LinC1Ev(&ray);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&ray, &v0, &v0x18, 0);
    func_02035394(&ray, ground);
    int r = _ZN7dBgW_Kc10DetectClsnER9dBgCh_Lin(self, &ray);
    if (r) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&v0x30, &ray);
        func_0203aa10(self, &v0x30, &v0x3c);
        _ZN5dBgPiaSERKS_(ground + 0x10, ray.data + 0x10);
        *(int *)(ground + 0x44) = v0x3c.y;
        *(unsigned char *)(ground + 0x48) = 1;
    }
    _ZN9dBgCh_LinD1Ev(&ray);
    return r;
}
}
