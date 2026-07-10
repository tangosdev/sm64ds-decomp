/* func_ov006_0211c720 @ 0x0211c720 (ov006, size 0x264)
 * Slot-machine style minigame state machine (state at +0x5608):
 * 0=init, 1=intro countdown (+0x5618), 2=result countdown (+0x5616) with
 * win/lose handling and 9999-capped win counter, 3=retry countdown.
 */
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

extern char *data_ov004_020beb68;

extern void func_ov004_020b0aa0(int);
extern void func_ov006_0211b954(char *);
extern void func_ov006_0211b80c(char *);
extern void func_ov006_0211b5e0(char *);
extern void func_ov006_0211b790(char *);
extern void func_ov006_0211b9c8(char *);
extern void func_ov006_02119ba4(char *);
extern void func_ov006_02119a88(char *);
extern void func_ov006_02119b00(char *);
extern void func_ov006_02119a18(char *);
extern void func_ov006_020c2594(char *);
extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int);
extern void func_ov004_020adb1c(int);
extern void func_ov006_020c2440(char *);
extern void func_ov006_020c2b8c(char *);

int func_ov006_0211c720(char *c)
{
    switch (*(int *)(c + 0x5608)) {
    case 0:
        *(int *)(c + 0x5608) = 1;
        break;
    case 1:
        if (*(u16 *)(c + 0x5618) != 0) {
            (*(u16 *)(int)(((long long)(int)(c + 0x5618)) & 0xFFFFFFFFFFFFFFFFLL))--;
            if (*(u16 *)(c + 0x5618) == 0) {
                func_ov004_020b0aa0(0x1d);
                if (*(u8 *)(c + 0xc4) == 0) {
                    *(u8 *)(c + 0xc3) = 1;
                    *(u8 *)(c + 0xc4) = 1;
                    *(u16 *)(c + 0xc0) = 0;
                }
            }
        }
        func_ov006_0211b954(c);
        func_ov006_0211b80c(c);
        func_ov006_0211b5e0(c);
        func_ov006_0211b790(c);
        break;
    case 2:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(((long long)(int)(c + 0x5616)) & 0xFFFFFFFFFFFFFFFFLL))--;
            if (*(u16 *)(c + 0x5616) == 0) {
                if (*(u8 *)(c + 0x5626) != 0) {
                    *(int *)(c + 0x50e0) = 0;
                    func_ov006_020c2594(c + 0x4f38);
                    if (*(u8 *)(c + 0x5626) == 3)
                        func_ov004_020b67f8();
                    func_ov004_020b0a54(0);
                    {
                        char *g = data_ov004_020beb68;
                        if (g != 0) {
                            if (*(int *)(g + 0xb4) < 9999)
                                (*(int *)(int)(((long long)(int)(g + 0xb4)) & 0xFFFFFFFFFFFFFFFFLL))++;
                            if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                                *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                        }
                    }
                    func_ov004_020adb1c(data_ov004_020beb68 != 0 ? *(int *)(data_ov004_020beb68 + 0xb4) : 0);
                    func_ov006_02119ba4(c);
                    func_ov006_02119a88(c);
                    *(u8 *)(c + 0xc3) = 0;
                } else {
                    *(int *)(c + 0x5608) = 3;
                    *(u16 *)(c + 0x5616) = 0x20;
                    func_ov006_0211b9c8(c);
                }
            }
        } else {
            func_ov006_02119b00(c);
            func_ov006_02119a18(c);
        }
        break;
    case 3:
        func_ov006_0211b954(c);
        func_ov006_0211b5e0(c);
        if (*(u16 *)(c + 0x5616) != 0) {
            (*(u16 *)(int)(((long long)(int)(c + 0x5616)) & 0xFFFFFFFFFFFFFFFFLL))--;
            if (*(s16 *)(c + 0x5616) <= 0) {
                *(int *)(c + 0x50e0) = 0;
                func_ov006_020c2440(c + 0x4f38);
                func_ov004_020b0a54(0x12);
                *(u8 *)(c + 0xc3) = 0;
                *(u16 *)(c + 0x5616) = 0;
            }
        }
        break;
    }

    func_ov006_020c2b8c(c + 0x4f38);
    return 1;
}
