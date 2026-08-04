struct P8 { int a; int b; };
extern void func_ov006_020debb4(char* a, int b);
extern void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int a4, int a5);
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2574(int arg0, int arg1);
extern void func_ov004_020b1e34(void* a, int b, int c, int d);
extern void func_ov006_020c29dc(char *c);

struct S8 { void *a; int b; };
extern struct S8 *data_ov006_0213c0d8[];
extern char data_ov006_02139df4[];

int func_ov006_020e0068(char *c)
{
    int list[3];
    int i;
    int n;
    int j;
    int x;
    int y;
    int k;
    int v;

    for (i = 0; i < 3; i++) {
        list[i] = i;
    }

    for (n = 3; n > 1; n--) {
        for (j = 0; j < n - 1; j++) {
            y = list[j + 1];
            x = list[j];
            if (*(int*)(c + x * 8 + 0x53ec) < *(int*)(c + y * 8 + 0x53ec)) {
                list[j] = y;
                list[j + 1] = x;
            }
        }
    }

    for (k = 0; k < 3; k++) {
        v = list[k];
        func_ov006_020debb4(c + 0x50e8, (char)v);
        func_ov006_020deed8((int)c,
            data_ov006_0213c0d8[*(int*)(c + v * 4 + 0x5434)][((int*)c + k)[0x1510]].a,
            *(int*)(c + v * 8 + 0x53e8),
            ((struct P8*)c + v)[0xa7d].b,
            *(int*)(c + v * 4 + 0x540c),
            *(unsigned char*)(c + v + 0x5462));
    }

    if (*(int*)(c + 0x5418) == 4 || *(int*)(c + 0x5418) == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20,
            (*(unsigned char*)(c + 0x5468) == 2) ? 4 : -1, -1);
    }

    func_ov004_020b2574(*(int*)(c + 0xa8), 1);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_020c29dc(c + 0x4f38);
    return 1;
}
