extern void func_0205283c(void* m, int s, int c);
extern short data_02082214[];

void Matrix4x3_FromRotationZ(void* m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_0205283c(m, data_02082214[a*2], data_02082214[a*2+1]);
}
