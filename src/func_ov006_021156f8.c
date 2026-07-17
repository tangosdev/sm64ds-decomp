typedef unsigned char u8;

extern int func_ov006_02111df4(char *o);
extern int func_ov006_0210e4c8(char *o);
extern int func_ov006_0210d898(char *o);
extern int func_ov006_021147d0(char *p);

int func_ov006_021156f8(char *p)
{
    int i;
    char *q;

    for (i = 0; i < *(int *)(p + 0x4668); i++) {
        if (*(u8 *)(((i >= 0xd) ? 0 : ((char **)(p + 0x4688))[i]) + 0x30) != 0) {
            if (func_ov006_02111df4((i >= 0xd) ? 0 : ((char **)(p + 0x4688))[i]) == 0)
                return 0;
        }
    }
    if (*(u8 *)(p + 0x595c) != 0)
        return 0;
    q = *(char **)(p + 0x4778);
    if (q != 0) {
        if ((*(int *)(q + 0x7c) > 0 ? 0 : *(u8 *)(q + 0x73)) != 0)
            return 0;
    }
    q = *(char **)(p + 0x477c);
    if (q != 0) {
        if (func_ov006_0210e4c8(q) != 0)
            return 0;
    }
    q = *(char **)(p + 0x4780);
    if (q != 0) {
        if (func_ov006_0210d898(q) == 0)
            return 0;
        *(u8 *)(*(char **)(p + 0x4780) + 0x44) = 1;
    }
    if (func_ov006_021147d0(p) != 0)
        return 0;
    return 1;
}
