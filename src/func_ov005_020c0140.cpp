//cpp
extern "C" {
extern int* data_0209f5bc[];
extern unsigned char data_0209b304[];
extern short data_0209d494[];
extern short data_0209d474[];

void func_ov005_020c0140(char* c) {
    int* g = data_0209f5bc[0];
    if ((*(int(***)(int*))g)[0x14/4](g) == 0) return;
    if (*(unsigned char*)(c + 0xac) != 0) return;
    if (*(int*)(c + 0x90) > 0) return;
    if (*(int*)(c + 0x94) > 0) return;
    if (*(int*)(c + 0x98) > 0) return;
    if (data_0209b304[0] == 0) {
        if (*(int*)(c + 0x50) >= 0) {
            *(int*)(((int)c + 0x50) & 0xFFFFFFFFFFFFFFFFULL) -= 8;
            if (*(int*)(c + 0x50) <= 0) {
                *(int*)(c + 0x50) = 0;
                *(unsigned char*)(c + 0x54) = 0;
            }
        }
    } else {
        if (*(int*)(c + 0x50) <= 0xb0) {
            *(int*)(((int)c + 0x50) & 0xFFFFFFFFFFFFFFFFULL) += 8;
            if (*(int*)(c + 0x50) >= 0xb0) {
                *(int*)(c + 0x50) = 0xb0;
                *(unsigned char*)(c + 0x54) = 0;
            }
        }
    }
    data_0209d494[0] = (short)*(int*)(c + 0x50);
    data_0209d474[0] = (short)*(int*)(c + 0x50);
}
}
