/* storage 0x0238F7B4 runtime 0x027C172C linked 0x188 main ARM. Single-switch
 * dispatcher (13 cases + default): guards via +0x1610, v2=(u16)(a1+12) kept
 * int, one switch (equalities; BGEs are range-lowering), shared tail F778.
 * 17 BLs; pools 0x182/0x185/0x186 numeric. New callee decls are void (int);
 * EB0C/F774 arities differ from their defs (dead-arg calls, F2F8 precedent). */

extern int *fn_0238E688(void);
extern void fn_0238E810(int);
extern void fn_0238E844(int);
extern void fn_0238E9C8(int);
extern void fn_0238EB0C(int);
extern void fn_0238EB40(int);
extern void fn_0238F5F8(int);
extern void fn_0238F740(int);
extern void fn_0238F62C(int);
extern void fn_0238EC40(int);
extern void fn_0238F050(int);
extern void fn_0238F388(int);
extern void fn_0238ED04(int);
extern void fn_0238EB74(int);
extern void fn_0238F774(int);
extern void fn_0238F778(int);

void fn_0238F7B4(int a1) {
    int *base;
    int *pb;
    int v2;

    base = fn_0238E688();
    if (*(int *)((char *)base + 0x1610) == 0)
        return;
    pb = *(int **)((char *)fn_0238E688() + 0x1610);
    if (*(unsigned short *)*pb == 1)
        return;
    v2 = *(unsigned short *)(a1 + 12);
    switch (v2) {
    case 0x84:
        fn_0238E810(a1);
        break;
    case 0x85:
        fn_0238E844(a1);
        break;
    case 0x86:
        fn_0238E9C8(a1);
        break;
    case 0x87:
        fn_0238EB0C(a1);
        break;
    case 0x88:
        fn_0238EB40(a1);
        break;
    case 0x8B:
        fn_0238F5F8(a1);
        break;
    case 0x8C:
        fn_0238F740(a1);
        break;
    case 0x8D:
        fn_0238F62C(a1);
        break;
    case 0x180:
        fn_0238EC40(a1);
        break;
    case 0x182:
        fn_0238F050(a1);
        break;
    case 0x184:
        fn_0238F388(a1);
        break;
    case 0x185:
        fn_0238ED04(a1);
        break;
    case 0x186:
        fn_0238EB74(a1);
        break;
    default:
        fn_0238F774(a1);
        break;
    }
    fn_0238F778(a1);
}
