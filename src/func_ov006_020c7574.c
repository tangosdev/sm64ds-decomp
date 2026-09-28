extern int _ZN16dMgJump3DMario_c19func_ov006_020c87d0Ev(char *p);
extern void _ZN16dMgJump3DMario_c19func_ov006_020c8658Ev(void *c);
extern void _ZN16dMgJump3DMario_c19func_ov006_020c76e0Ev(char *c);
extern char *data_ov006_02140420;
extern int data_ov006_02140418;
extern int data_ov006_02140428;
extern char *data_ov006_021403f4[];
extern int data_ov006_0213b010;

int func_ov006_020c7574(char *base, int count)
{
    int i;
    int off;
    data_ov006_02140420 = base;
    data_ov006_02140418 = count;
    data_ov006_02140428 = count;
    i = 0;
    if (count > 0) {
        off = i;
        do {
            char *q;
            if (!_ZN16dMgJump3DMario_c19func_ov006_020c87d0Ev(data_ov006_02140420 + off)) return 0;
            _ZN16dMgJump3DMario_c19func_ov006_020c8658Ev(data_ov006_02140420 + off);
            _ZN16dMgJump3DMario_c19func_ov006_020c76e0Ev(data_ov006_02140420 + off);
            q = data_ov006_02140420 + off;
            if (i < 3) data_ov006_021403f4[i] = q;
            i++;
            off += 0xb8;
        } while (i < data_ov006_02140418);
    }
    data_ov006_0213b010 = -0x1e00;
    return 1;
}
