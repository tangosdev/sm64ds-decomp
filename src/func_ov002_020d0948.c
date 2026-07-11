extern short data_02082214[];
extern int data_ov002_021101b4[];
extern void _ZN6Player11ChangeStateERNS_5StateE(int self, int state);

void func_ov002_020d0948(int self)
{
    int base = (short)(*(short*)(self + 0x8e) + 0x8000);
    if (*(unsigned char*)(self + 0x703) != 0) {
        int idx = (unsigned short)base >> 4;
        int* p0 = (int*)(((long long)(int)(self + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        int* p1 = (int*)(((long long)(int)(self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
        int* p2 = (int*)(((long long)(int)(self + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
        *p0 = data_02082214[idx * 2] * 0x110 + *p0;
        *p1 = *p1 - 0x12c000;
        *p2 = data_02082214[idx * 2 + 1] * 0x110 + *p2;
    } else {
        int idx = (unsigned short)base >> 4;
        int* p0 = (int*)(((long long)(int)(self + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        int* p1 = (int*)(((long long)(int)(self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
        int* p2 = (int*)(((long long)(int)(self + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
        *p0 = *p0 + (data_02082214[idx * 2] << 6);
        *p1 = *p1 - 0x64000;
        *p2 = *p2 + (data_02082214[idx * 2 + 1] << 6);
    }
    *(int*)(self + 0x98) = -0x8000;
    _ZN6Player11ChangeStateERNS_5StateE(self, (int)data_ov002_021101b4);
}
