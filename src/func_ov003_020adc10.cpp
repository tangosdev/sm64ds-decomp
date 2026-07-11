//cpp
extern "C" {
extern char data_0208e4b8[];
extern char data_02092680[];
extern char data_ov003_020b1650[];
void *_ZN9ActorBasenwEj(unsigned int);
void _ZN9ActorBaseC1Ev(void *);
void *func_ov003_020adc10(void);
}

void *func_ov003_020adc10(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x54);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(void **)p = (void *)data_0208e4b8;
        *(void **)p = (void *)data_02092680;
        unsigned char *bytePtr = (unsigned char *)(int)((unsigned long long)((int)p + 0x13) & 0xFFFFFFFFFFFFFFFFULL);
        *bytePtr |= 1;
        *bytePtr |= 4;
        *(void **)p = (void *)data_ov003_020b1650;
    }
    return p;
}
