extern void* data_ov001_020ad634[];
extern unsigned char data_ov001_020ad630[];
extern unsigned char data_ov001_020ad628[];
extern void func_ov001_020aa6cc(int r0);

#pragma opt_common_subs off

void func_ov001_020ab228(char* c, char* a1, int idx, int a3, unsigned char a5){
    *(unsigned char*)(c+0x18) = (unsigned char)idx;
    *(int*)(c+4) = (int)a1;
    *(int*)(c+8) = *(int*)(a1+4);
    *(unsigned char*)(c+0x19) = (unsigned char)a3;
    *(int*)(c+0x10) = (int)data_ov001_020ad634[idx];
    if(data_ov001_020ad634[idx]) *(int*)((char*)data_ov001_020ad634[idx]+0xc) = (int)c;
    data_ov001_020ad634[idx] = c;
    data_ov001_020ad630[idx]++;
    if(a3==3) data_ov001_020ad628[idx] |= 8;
    *(unsigned char*)(c+0x1a) = 0;
    *(unsigned char*)(c+0x1b) = 0;
    {
        unsigned char *p1b = (unsigned char *)(int)(c + 0x1b);
        *p1b = (*p1b & ~1) | (a5 & 1);
    }
    if(a5!=0 || a3==3) func_ov001_020aa6cc(idx);
    *(int*)(c+0x14) = -1;
}
