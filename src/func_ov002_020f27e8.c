extern int data_ov002_02100140[];
extern void func_ov002_020f2790(char *c, int p1, int p2, int p3, short p4);
extern void func_ov002_020f2984(char *p);

void func_ov002_020f27e8(char *c)
{
    if (*(unsigned char *)(c + 0x1ed) == 0) return;

    *(unsigned char *)(((long long)(int)(c + 0x1ec)) & 0xFFFFFFFFFFFFFFFFLL) += 1;

    if (*(int *)(c + 0x1e4) != 0x1000) {
        if (*(unsigned char *)(c + 0x1ec) == data_ov002_02100140[*(unsigned short *)(c + 0x1ea) & 3]) {
            *(int *)(((long long)(int)(c + 0x1e4)) & 0xFFFFFFFFFFFFFFFFLL) += 0x199;
            if (*(int *)(c + 0x1e4) >= 0x1000) *(int *)(c + 0x1e4) = 0x1000;
        }
    }

    if (*(short *)(c + 0x1e8) != 0x800) {
        if (*(unsigned short *)(c + 0x1ea) >= 5) {
            if ((*(unsigned char *)(c + 0x1ec) & 3) == 3) {
                *(short *)(((long long)(int)(c + 0x1e8)) & 0xFFFFFFFFFFFFFFFFLL) += 0x199;
                if (*(short *)(c + 0x1e8) >= 0x800) *(short *)(c + 0x1e8) = 0x800;
            }
        }
    }

    func_ov002_020f2790(c, 0x80, 0x60, *(int *)(c + 0x1e4), *(short *)(c + 0x1e8));
    func_ov002_020f2790(c, 0x80, 0x60, *(int *)(c + 0x1e4), *(short *)(c + 0x1e8));

    if (*(unsigned char *)(c + 0x1ec) >= 0x1e) {
        *(unsigned char *)(c + 0x1ec) = 0;
        *(unsigned short *)(((long long)(int)(c + 0x1ea)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }
    if (*(unsigned short *)(c + 0x1ea) < 0xa) return;
    func_ov002_020f2984(c);
}
