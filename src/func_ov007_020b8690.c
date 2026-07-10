// Clears two ov007 state globals, runs func_ov007_020c94e4, then releases
// two cached handles via func_ov007_020c44c4 (zeroing each afterwards).
extern int data_ov007_0210345c;
extern short data_ov007_02103458;
extern char* data_ov007_0210346c;
extern char* data_ov007_02103460;
extern void func_ov007_020c94e4(void);
extern int func_ov007_020c44c4(char* p);

#pragma opt_propagation off
void func_ov007_020b8690(void) {
    int *pa = (int *)(int)((long long)(int)&data_ov007_0210345c & 0xFFFFFFFFFFFFFFFFLL);
    short *pb = (short *)(int)((long long)(int)&data_ov007_02103458 & 0xFFFFFFFFFFFFFFFFLL);
    int v = 0;
    *pa = v;
    *pb = v;
    func_ov007_020c94e4();
    if (data_ov007_0210346c != 0) {
        func_ov007_020c44c4(data_ov007_0210346c);
        data_ov007_0210346c = 0;
    }
    if (data_ov007_02103460 != 0) {
        func_ov007_020c44c4(data_ov007_02103460);
        data_ov007_02103460 = 0;
    }
}
