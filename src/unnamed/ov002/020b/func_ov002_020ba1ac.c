extern void func_ov002_020ba4d8(char* c, int i);
extern int func_02012310(int, int, int);
extern int data_0209b454;

void func_ov002_020ba1ac(char* self) {
    int diff;
    if (*(int*)(self + 0x33c) == 0) return;
    if (*(int*)(self + 0x33c) == 1) {
        if (*(unsigned char*)(self + 0x350) == 0) {
            func_ov002_020ba4d8(self, 3);
            return;
        }
    }
    diff = *(unsigned short*)(self + 0x33a) - *(unsigned short*)(self + 0x338);
    if (!(data_0209b454 & 0x4000000)) {
        if (diff == 0x2d) {
            *(int*)(self + 0x330) = 0;
        } else if (diff < 0x2d) {
            *(int*)(self + 0x330) = func_02012310(*(int*)(self + 0x330), 0x39, 0);
        } else {
            *(int*)(self + 0x330) = func_02012310(*(int*)(self + 0x330), 0x38, 0);
        }
    }
    if (*(unsigned short*)(self + 0x338) > *(unsigned short*)(self + 0x33a)) {
        func_ov002_020ba4d8(self, 4);
        return;
    }
    if (*(int*)(self + 0x348) == 0) return;
    if (*(int*)(*(int*)(self + 0x348) + 0x440) != 5) return;
    func_ov002_020ba4d8(self, 4);
}
