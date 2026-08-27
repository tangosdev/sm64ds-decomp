extern void func_02052800(void* m, int s, int c);
extern short data_02082214[];

void Matrix4x3_FromRotationX(void* m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052800(m, data_02082214[a*2], data_02082214[a*2+1]);
}
