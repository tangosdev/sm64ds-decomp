
extern int func_02012790(int);
extern int func_ov005_020bff4c(char* c);
extern int RandomIntInternal(int* seed);
extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern void ExitMinigameMenu(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
extern void func_ov005_020c0878(char* c);
extern void func_ov005_020c06cc(char* c);
extern void func_ov005_020c0378(char* c);
extern void func_ov005_020c0250(char* c);
extern void func_ov005_020c0140(char* c);

extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_0209b300;
extern int data_0209d4b8;
extern int data_0209e650;
extern unsigned char data_0209b304;

int func_ov005_020c14a0(char* c) {
    if ((data_020a0e5a[data_020a0e40 << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (data_0209b300 == 1) {
        func_ov005_020bff4c(c);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8);
    RandomIntInternal(&data_0209e650);

    if (*(int*)(c+0x90) > 0) {
        *(int*)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFFULL) -= 1;
        if (*(int*)(c+0x90) == 0) {
            data_0209b304 = 0;
            *(unsigned char*)(c+0x54) = 1;
        }
    } else if (*(int*)(c+0x94) > 0) {
        *(int*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFFULL) -= 1;
        if (*(int*)(c+0x94) == 0) {
            data_0209b304 = 1;
            *(unsigned char*)(c+0x54) = 1;
        }
    } else if (*(int*)(c+0x98) > 1) {
        *(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFULL) -= 1;
        if (*(int*)(c+0x98) == 1) {
            _ZN5Scene20SetAndStopColorFaderEv();
            ExitMinigameMenu();
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
            *(unsigned char*)(c+0xac) = 1;
            return 1;
        }
    } else {
        *(int*)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFFULL) += 1;
        if (*(int*)(c+0x8c) >= 0x40) *(int*)(c+0x8c) = 0;
        *(int*)(((int)c + 0x9c) & 0xFFFFFFFFFFFFFFFFULL) += 1;
        if (*(int*)(c+0x9c) >= 0x40) *(int*)(c+0x9c) = 0;
    }
    func_ov005_020c0878(c);
    func_ov005_020c06cc(c);
    func_ov005_020c0378(c);
    func_ov005_020c0250(c);
    func_ov005_020c0140(c);
    return 1;
}
