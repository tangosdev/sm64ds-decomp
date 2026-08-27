extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void* thiz, unsigned int op, int b);

void func_ov060_02118690(char* c) {
    Matrix4x3_FromTranslation(c + 0xf0, *(int*)(c + 0x5c) >> 3, *(int*)(c + 0x60) >> 3, *(int*)(c + 0x64) >> 3);
    _ZN9ModelBase12ApplyOpacityEj(c + 0xd4, (unsigned char)((int)*(unsigned char*)(c + 0x1ae) >> 3), 1);
}
