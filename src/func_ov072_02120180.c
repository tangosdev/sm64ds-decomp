extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov072_02122bcc[];
int func_ov072_02120180(char *c) {
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x124, (void*)data_ov072_02122bcc[1], 0, 0x1000, 0);
    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0xa0) = 0;
    *(unsigned char*)(c + 0x334) = 0;
    *(int*)(c + 0x330) = 3;
    return 1;
}
