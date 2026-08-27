extern int data_ov002_020ff120[];
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(int a, int b, void *c);

void func_ov002_020d91b8(unsigned char *self, int a) {
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(self[0x6d9], data_ov002_020ff120[a & 1], self + 0x74);
}
