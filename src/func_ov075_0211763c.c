extern int func_0203da9c(void);
extern int func_ov001_020ab450(void *p, int x);
extern int func_ov075_02116d9c(void *self, int id);
extern void SetControllerMode(unsigned char mode);
extern void func_02020028(int a, unsigned char b);
extern void func_ov001_020ab41c(void *p);
extern void func_ov075_02116c38(void *self);
extern void func_02012790(int a);
extern void SetBg2Offset(int a, int b);
extern unsigned short data_020a0e5a[];

void func_ov075_0211763c(void *thiz)
{
    unsigned char *s = (unsigned char *)thiz;
    int w = func_0203da9c();

    if (func_ov001_020ab450(s + 0xdc, -1) != 0
        || func_ov001_020ab450(s + 0x100, -1) != 0
        || (data_020a0e5a[w << 1] & 8) != 0
        || (data_020a0e5a[w << 1] & 4) != 0
        || (func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 3)) {
        int i;
        unsigned char *p = s;
        for (i = 0; i <= 2; i++, p += 0x24) {
            if (p[0x80] != 0) {
                SetControllerMode(i);
                func_02020028(w, i);
            }
        }
        func_ov001_020ab41c(s + 0xdc);
        s[0x281] = 3;
        func_ov075_02116c38(s);
        func_02012790(0x11a);
        return;
    }

    if (func_ov001_020ab450(s + 0x70, -1) != 0
        || (func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 0)) {
        s[0x281] = 0;
        if (s[0x80] != 0) {
            SetControllerMode(0);
            func_02020028(w, 0);
            func_ov075_02116c38(s);
            func_02012790(0x11a);
        } else {
            s[0xa4] = 0;
            s[0xc8] = 0;
            *(int *)(s + 0x268) = s[0x281];
            SetBg2Offset(0, 0);
            func_02012790(0x5b);
        }
        func_ov001_020ab41c(s + 0x70);
        return;
    }

    if (func_ov001_020ab450(s + 0x94, -1) != 0
        || (func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 1)) {
        s[0x281] = 1;
        if (s[0xa4] != 0) {
            SetControllerMode(1);
            func_02020028(w, 1);
            func_ov075_02116c38(s);
            func_02012790(0x11a);
        } else {
            s[0x80] = 0;
            s[0xc8] = 0;
            *(int *)(s + 0x268) = s[0x281];
            SetBg2Offset(0x100, 0);
            func_02012790(0x5b);
        }
        func_ov001_020ab41c(s + 0x94);
        return;
    }

    if (func_ov001_020ab450(s + 0xb8, -1) != 0
        || (func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 2)) {
        s[0x281] = 2;
        if (s[0xc8] != 0) {
            SetControllerMode(2);
            func_02020028(w, 2);
            func_ov075_02116c38(s);
            func_02012790(0x11a);
        } else {
            s[0x80] = 0;
            s[0xa4] = 0;
            *(int *)(s + 0x268) = s[0x281];
            SetBg2Offset(0, 0x100);
            func_02012790(0x5b);
        }
        func_ov001_020ab41c(s + 0xb8);
        return;
    }
}
