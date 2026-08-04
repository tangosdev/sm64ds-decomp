extern int* data_ov006_02142aa4[];
extern int data_ov006_0213e2dc;
extern int data_ov006_021428d0;
extern void func_ov006_02107d58(int* p);
void func_ov006_02107b14(void) {
    int i;
    for (i = 0; i < 4; i++) {
        func_ov006_02107d58(data_ov006_02142aa4[i]);
        *(short*)((char*)data_ov006_02142aa4[i] + 0x14) = (short)i;
    }
    data_ov006_0213e2dc = -1;
    data_ov006_021428d0 = 0;
}
