extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern char data_ov002_0211092c;
extern char RED_NUMBER_MODEL_PTR;
extern char data_ov002_0211093c;

int func_ov002_020eabcc(char* c) {
    if (*(unsigned char*)(c + 0x1d8) != 0) {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
        _ZN13SharedFilePtr7ReleaseEv(&RED_NUMBER_MODEL_PTR);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211093c);
    }
    return 1;
}
