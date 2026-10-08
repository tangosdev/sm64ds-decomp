typedef struct {
    int field0;
    int field4;
    int field8;
    short fieldc;
    short fielde;
    int field10;
    int field14;
    int field18;
} Obj;
extern Obj *data_ov007_02103360;
extern int func_ov007_020c3df4(int a, int b);
extern int func_ov007_020c9388(int v);
extern int func_ov007_020c44e4(int v);
void func_ov007_020b30b0(void);
void func_ov007_020b30b0(void) {
    int *sub;
    if (data_ov007_02103360 != 0) return;
    data_ov007_02103360 = (Obj *)func_ov007_020c3df4(0, 0x1c);
    data_ov007_02103360->field0 = func_ov007_020c9388(0xff);
    data_ov007_02103360->field4 = func_ov007_020c44e4(1);
    data_ov007_02103360->field8 = 1;
    data_ov007_02103360->field14 = -1;
    data_ov007_02103360->field10 = 0;
    data_ov007_02103360->field18 = -1;
    data_ov007_02103360->fielde = 0;
    data_ov007_02103360->fieldc = data_ov007_02103360->fielde;
    sub = (int *)data_ov007_02103360->field4;
    *((signed char *)sub + 0x4e) = 0x21;
    sub[0x1b] = 1;
    sub[0] |= 4;
    *((signed char *)sub + 0x7c) = 0x14;
    sub[4] = 0xffffef01;
}
