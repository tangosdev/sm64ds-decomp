void func_ov007_020ae810(char* c) {
    short* ptr1 = *(short**)(c + 0x24);
    *(int*)(c + 8) = (int)ptr1[1] << 12;
    
    short* ptr2 = *(short**)(c + 0x24);
    *(int*)(c + 0xc) = (int)ptr2[2] << 12;
}
