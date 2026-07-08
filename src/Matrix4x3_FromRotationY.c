extern void func_02052820(void* m, int s, int c);
extern short SINE_TABLE[];

void Matrix4x3_FromRotationY(void* m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052820(m, SINE_TABLE[a*2], SINE_TABLE[a*2+1]);
}
