/* storage 0x02392BBC runtime 0x027C4B34 size 0x98 main. Pool +0x98: 027C4AAC. */

extern int *fn_0238E688(void);
extern void fn_02382724(int);
extern void fn_02382810(int, int, int, int);
extern void fn_02392B34(unsigned int);

void fn_02392BBC(void) {
    int *base;
    int *obj;
    unsigned short kind;

    base = fn_0238E688();
    obj = (int *)base[1412];
    kind = *(unsigned short *)(*obj + 0xE2);
    if (kind == 1) {
        if (obj[52] != 0)
            fn_02382724((int)(obj + 52));
        fn_02382810((int)(obj + 52), 0xD1, (int)fn_02392B34, 0xD1);
    } else if (kind == 2) {
        *(int *)(*obj + 0x24) = 0;
        if (obj[52] != 0)
            fn_02382724((int)(obj + 52));
        fn_02382810((int)(obj + 52), 0xC8, (int)fn_02392B34, 0xC8);
        *(int *)(*obj + 0xD4) = 0;
    }
}
