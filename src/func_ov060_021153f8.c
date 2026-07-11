typedef unsigned char u8;

extern int func_ov060_02111f08(void *arg0);
extern int func_ov060_02115744(void *c);
extern int func_ov060_02115718(void *c);
extern int func_ov060_021156ec(void *c);
extern void func_ov060_02111cc0(char *c, int idx, int extra);
extern void func_ov060_02115518(void *c);

#define P423(c) ((u8 *)(((long long)(int)((c) + 0x423)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov060_021153f8(char *c)
{
    u8 state;
    func_ov060_02111f08(c);
    state = *(u8 *)(c + 0x423);
    if (state == 0) {
        if (func_ov060_02115744(c) == 0) return;
        {
            u8 *p = P423(c);
            *p = *p + 1;
        }
        return;
    }
    if (state <= 2) {
        if (func_ov060_02115718(c) == 0) return;
        {
            u8 *p = P423(c);
            *p = *p + 1;
            if (*(u8 *)(c + 0x414) == 2) {
                *p = *p + 1;
            }
        }
        return;
    }
    if (state == 3) {
        if (func_ov060_021156ec(c) == 0) return;
        func_ov060_02111cc0(c, 0x10, 0);
        {
            u8 *p = P423(c);
            *p = *p + 1;
        }
        return;
    }
    func_ov060_02115518(c);
}
