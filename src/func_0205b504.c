typedef struct UnkStruct_020a7f60 {
    char pad0[8];
    unsigned char unk8;
    char pad9[3];
} UnkStruct_020a7f60;

extern UnkStruct_020a7f60 data_020a7f60[];

void func_0205b504(int idx)
{
    *(unsigned char *)((long long)((int)&data_020a7f60[idx] + 8) & 0xffffffffffffffffLL) += 1;
}
