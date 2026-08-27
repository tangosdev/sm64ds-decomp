typedef struct { int m0, m4, m8; } Triple;

extern int data_ov006_021405c8[];
extern int data_ov006_0214054c;
extern void *data_ov006_02140564;
extern int data_ov006_02140588;
extern int data_ov006_0214058c;
extern int data_ov006_0214055c;
extern char *data_ov006_02141a50;
extern char *data_ov006_02141a40;
extern int data_ov006_02140584;
extern Triple data_ov006_02140778[];
extern Triple data_020a0ebc;

extern void _Z11UpdateAngleRssis(short *, short, int, short);
extern int _ZN9Animation8FinishedEv(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _Z14ApproachLinearRiii(int *, int, int);
extern void _Z15ApproachLinear2Rsss(short *, short, short);
extern void func_02012718(void *, int);
extern void func_ov006_020bfff8(char *, void *, int *, int *);
extern int func_ov004_020b04c0(void);
extern void func_ov006_02120d0c(int, int);
extern void func_ov006_020ccd78(char *);

void func_ov006_020cb2b4(void *self)
{
    char *c = (char *)self;
    int va, vb;
    short idx;
    int lim;
    int u;
    int i;

    idx = *(short *)(c + 0x52);
    lim = data_ov006_021405c8[idx];
    if (*(int *)(c + 0x20) <= lim) {
        *(int *)(c + 0x20) = lim;
        *(int *)(c + 0x38) = 0;
    }

    if (*(int *)(c + 0xcc) == data_ov006_0214054c) {
        _Z11UpdateAngleRssis((short *)(c + 0x4a), 0, 2, 0x1000);
        if (_ZN9Animation8FinishedEv(c + 0xbc) == 0)
            return;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_02140564, 0, 0x800, 0);
        return;
    }

    if (*(int *)(c + 0x1c) > 0) {
        _Z14ApproachLinearRiii((int *)(c + 0x34), 0x4000, 0x800);
        _Z15ApproachLinear2Rsss((short *)(c + 0x4a), 0x4000, 0x1000);
    } else {
        _Z14ApproachLinearRiii((int *)(c + 0x34), -0x4000, 0x800);
        _Z15ApproachLinear2Rsss((short *)(c + 0x4a), -0x4000, 0x1000);
    }

    u = *(int *)(c + 0x1c);
    if (u <= 0x90000 && u >= -0x90000)
        return;

    func_02012718((void *)0x1be, 0x100000);
    _Z14ApproachLinearRiii(&data_ov006_02140588, 0x270f, 1);
    data_ov006_0214058c -= 1;
    data_ov006_0214055c -= 1;

    if (*(int *)(c + 0x20) > 0) {
        func_ov006_020bfff8(data_ov006_02141a50, c + 0x1c, &va, &vb);
        vb = vb - (func_ov004_020b04c0() + 0xc0);
    } else {
        func_ov006_020bfff8(data_ov006_02141a40, c + 0x1c, &va, &vb);
    }
    vb -= 0x20;
    func_ov006_02120d0c(vb << 12, (short)data_ov006_02140588);

    data_ov006_02140584 += 1;
    if (data_ov006_02140584 >= 6)
        data_ov006_02140584 = 0;

    i = data_ov006_02140584;
    {
        Triple *p = &data_ov006_02140778[i];
        *(int *)(c + 0x1c) = p->m0;
        *(int *)(c + 0x20) = p->m4;
        *(int *)(c + 0x24) = p->m8;
    }
    *(int *)(c + 0x34) = data_020a0ebc.m0;
    *(int *)(c + 0x38) = data_020a0ebc.m4;
    *(int *)(c + 0x3c) = data_020a0ebc.m8;

    func_ov006_020ccd78(c);
}
