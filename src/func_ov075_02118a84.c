extern int func_0203d8fc(void);
extern void func_0201a244(void *cb, int a, int b, int c, int d);
extern int func_0201a1bc(void);
extern void func_0203d930(void);
extern void func_ov075_02116c8c(void *p);
extern void func_ov075_02118bf8(void);

extern int DP_STATE;
extern unsigned char data_ov075_0211d5bc;
extern unsigned char data_0209b2e4;
extern int GAME_SPEED_RELATED;
struct P2 { int a, b; };
extern struct P2 data_02086b58;

void func_ov075_02118a84(char *r4)
{
    if (*(int *)(r4 + 0x268) == 0) {
        if (func_0203d8fc() != 0)
            *(int *)(r4 + 0x268) = 1;
    }
    if (*(unsigned char *)(r4 + 0x282) == 0) {
        if (DP_STATE != 0) {
            if (data_ov075_0211d5bc == 0) {
                func_0201a244((void *)&func_ov075_02118bf8, data_0209b2e4, 0xf, 0, 0x1000);
                data_ov075_0211d5bc = 1;
            }
            if (data_ov075_0211d5bc != 0 && func_0201a1bc() != 0) {
                *(unsigned char *)(r4 + 0x282) = 1;
                data_ov075_0211d5bc = 0;
                func_0203d930();
            }
        } else {
            *(unsigned char *)(r4 + 0x282) = 2;
        }
    }
    if (*(unsigned char *)(r4 + 0x282) == 1 && *(int *)(r4 + 0x268) != 0) {
        if (func_0203d8fc() == 0)
            *(unsigned char *)(r4 + 0x282) = 2;
    }
    if (*(int *)(r4 + 0x264) > 0) {
        int *p = (int *)(((long long)(int)(r4 + 0x264)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p - GAME_SPEED_RELATED;
    }
    if (*(unsigned char *)(r4 + 0x282) < 2)
        return;
    if (*(int *)(r4 + 0x264) > 0)
        return;
    func_ov075_02116c8c(r4);
    *(struct P2 *)(r4 + 0x5c) = data_02086b58;
}