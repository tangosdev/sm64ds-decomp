struct S { int a, b; };
extern int func_ov006_02110c08(int x, struct S *s);

int func_ov006_021120d4(char *a, struct S *r7)
{
    struct S loc;
    int i;
    if (r7->b >= 0x90000) return 0;
    i = 0;
    if (*(int *)(*(char **)(a + 4) + 0x4674) > 0) {
        do {
            char *base;
            int x;
            loc.a = r7->a;
            loc.b = r7->b;
            base = *(char **)(a + 4);
            if (i >= *(int *)(base + 0x4674))
                x = 0;
            else
                x = ((int *)(base + 0x4740))[i];
            if (func_ov006_02110c08(x, &loc) != 0) return 1;
            i++;
        } while (i < *(int *)(*(char **)(a + 4) + 0x4674));
    }
    return 0;
}
