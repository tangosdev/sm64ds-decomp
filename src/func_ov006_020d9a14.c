extern void func_ov004_020af68c(void *a0, int a1, int a2, int a3, int a4);
extern int func_ov006_020da834(char *p);
extern int func_ov006_020da860(char *p, int v);
extern void **data_ov006_0213bd30[];
extern void *data_ov006_0213406c[];

#define F(off) (*(short *)(this + 0x5300 + (off)))

void func_ov006_020d9a14(char *this)
{
    short v = F(0x88);
    if (v > 3 && v < 0x11) {
        int sb = 0xb0;
        int i;
        for (i = 0; i < 6; i++) {
            int sel = 0;
            if ((i == F(0x92) && i == F(0x8e)) || (i == F(0x94) && i == F(0x90))) {
                if (F(0x96) & 0x10) sel = 1; else sel = 2;
            } else if (i == F(0x92) || i == F(0x94)) {
                if (F(0x96) & 0x10) sel = 2;
            } else if (i == F(0x8e) || i == F(0x90)) {
                if (F(0x96) & 0x10) sel = 1;
            }
            func_ov004_020af68c(data_ov006_0213bd30[i][sel], 0x14, sb, -1, -1);
            sb -= 0x10;
        }
    }
    if (F(0x88) != 4) return;
    if (func_ov006_020da834(this + 0x51a8) == 0) return;
    if (func_ov006_020da860(this + 0x51a8, 2) == 0)
        func_ov004_020af68c(data_ov006_0213406c[0], 0x80, 0x58, -1, -1);
    else
        func_ov004_020af68c(data_ov006_0213406c[1], 0x80, 0x58, -1, -1);
}
