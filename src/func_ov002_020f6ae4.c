extern unsigned int ReadUnalignedInt(unsigned char *p);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int e);

int func_ov002_020f6ae4(char *thiz, unsigned char *p) {
    unsigned int r = ReadUnalignedInt(p);
    *(unsigned int *)(thiz + 0xe4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(thiz + 0xe4), 3, r, thiz + 0x74, 0);
    return 1;
}
