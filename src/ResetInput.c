typedef unsigned char u8;
extern u8 GetControllerMode(int a0);
extern void func_0205a588(void *a, int b, int c);
extern u8 NUM_PLAYERS;

struct Ctrl {
    char pad[0x12];
    u8 f12;     /* 0x12 */
    char pad2[0x16 - 0x13];
    u8 f16;     /* 0x16 */
    char pad3[0x18 - 0x17];
};

extern struct Ctrl INPUT_ARR[];
extern u8 CURRENT_GAMEMODE;

void ResetInput(void)
{
    struct Ctrl *p = INPUT_ARR;
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        u8 mode;
        int m = (int)(CURRENT_GAMEMODE == 1);
        if (m == 0)
            mode = p->f16;
        else
            mode = GetControllerMode(i);
        func_0205a588(p, 0, 0x18);
        p->f16 = mode;
        p->f12 = 5;
        p++;
    }
}
