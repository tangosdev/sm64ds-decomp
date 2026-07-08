extern void func_0205283c(void* m, int s, int c);
extern short SINE_TABLE[];

void Matrix4x3_FromRotationZ(void* m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_0205283c(m, SINE_TABLE[a*2], SINE_TABLE[a*2+1]);
}
