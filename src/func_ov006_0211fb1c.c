typedef unsigned char u8;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

void func_ov006_0211fb1c(char* c) {
    int i = data_020a0e40;
    if (data_020a0de8[(unsigned int)i * 4] != 0 && *(u8*)(c + 0x4c1d) == 0) {
        *(int*)(c + 0x4bec) = data_020a0dea[i * 4];
        *(int*)(c + 0x4bf0) = data_020a0deb[i * 4];
        *(int*)(c + 0x4bf4) = *(int*)(c + 0x4bec);
        *(int*)(c + 0x4bf8) = *(int*)(c + 0x4bf0);
        *(u8*)AT(c, 0x4c1d) += 1;
    }
    if (data_020a0de8[(unsigned int)data_020a0e40 * 4] == 0) {
        *(u8*)(c + 0x4c1d) = 0;
    }
    if (*(u8*)(c + 0x4c1d) == 0) return;
    *(int*)(c + 0x4bec) = data_020a0dea[(unsigned int)data_020a0e40 * 4];
    *(int*)(c + 0x4bf0) = data_020a0deb[(unsigned int)data_020a0e40 * 4];
}
