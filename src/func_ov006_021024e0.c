#pragma opt_strength_reduction off
extern unsigned char data_ov006_0212ebb8[];
void func_ov006_021024e0(int this) {
    int n = data_ov006_0212ebb8[*(int*)(this + 0x5000 + 0x668)];
    int i = 0;
    if (n <= 0) return;
    do {
        char *b = (char*)(this + i * 0x40);
        *(unsigned char*)(b + 0x5294) = 1;
        *(unsigned char*)(b + 0x5296) = 0;
        *(unsigned char*)(b + 0x529b) = 0;
        *(short*)(b + 0x5292) = 0x20;
        *(int*)(b + 0x528c) = 0;
        *(int*)(b + 0x5274) = 0;
        *(int*)(b + 0x5278) = 0;
        *(unsigned char*)(b + 0x529c) = 0;
        *(unsigned char*)(b + 0x529d) = 0;
        i++;
    } while (i < n);
}
