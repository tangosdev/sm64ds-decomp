void func_ov006_02107c50(int* obj, int arg);
void func_ov006_02107b94(int* obj);
extern int data_ov006_0213e2dc;
extern int* data_ov006_02142aa4[];

void func_ov006_021079cc(void){
    int n = data_ov006_0213e2dc;
    int arg;
    int i;
    if (n < 0) return;
    n = n + 1;
    if (n > 4) n = 4;
    arg = 0x28;
    for (i = 0; i < n; i++) {
        int* o = data_ov006_02142aa4[i];
        if (*(short*)((char*)o + 0x16) > 0) {
            func_ov006_02107c50(o, arg);
            arg += 0x24;
        }
    }
    if (data_ov006_0213e2dc >= 4) return;
    func_ov006_02107b94(data_ov006_02142aa4[data_ov006_0213e2dc]);
}
