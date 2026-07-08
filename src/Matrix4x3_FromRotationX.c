extern void func_02052800(void* m, int s, int c);
extern short SINE_TABLE[];

void Matrix4x3_FromRotationX(void* m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052800(m, SINE_TABLE[a*2], SINE_TABLE[a*2+1]);
}
