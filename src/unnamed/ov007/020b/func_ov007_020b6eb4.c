typedef struct ARMMathState { int pad[7]; } ARMMathState;

extern void ARMMathSaveState(ARMMathState* s);
extern void ARMMathLoadState(ARMMathState* s);
extern void func_ov007_020bf690(void);
extern void func_ov007_020bd4e8(void);
extern void func_ov007_020bd648(void);
extern void func_ov007_020b72a0(void);
extern char* data_ov007_02103430;
extern char* data_ov007_0210342c;

int func_ov007_020b6eb4(void) {
    ARMMathState s;
    if (data_ov007_02103430 != 0) {
        ARMMathSaveState(&s);
        func_ov007_020bf690();
        if (*(int*)(data_ov007_0210342c + 0x18) != 0) {
            func_ov007_020bd4e8();
            *(int*)(data_ov007_0210342c + 0x18) = 0;
        }
        if (*(int*)(data_ov007_0210342c + 0x1c) != 0) {
            func_ov007_020bd648();
            *(int*)(data_ov007_0210342c + 0x1c) = 0;
        }
        func_ov007_020b72a0();
        ARMMathLoadState(&s);
    }
    return 1;
}
