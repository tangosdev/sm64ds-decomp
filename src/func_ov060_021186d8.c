extern int AddSpikeBomb(void* p);
void func_ov060_021186d8(unsigned char* c) {
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(unsigned char*)(c + 0x1ae) = 0xff;
    *(int*)(c + 0x170) = 0;
    (*(int *)(((int)c + 0x13c) & 0xFFFFFFFFFFFFFFFF)) &= ~1;
    *(short*)(c + 0x1ac) = 0;
    *(int*)(c + 0x1a8) = AddSpikeBomb(c);
}
