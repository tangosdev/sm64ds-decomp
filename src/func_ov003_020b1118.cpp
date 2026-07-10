//cpp
extern "C" {
extern void *data_0208e4b8;
extern void *_ZTV5Stage;
extern void *data_ov003_020b179c;
void *_ZN9ActorBasenwEj(unsigned int);
void _ZN9ActorBaseC1Ev(void *);
void *func_ov003_020b1118(void);
}

void *func_ov003_020b1118(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x98);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(void **)p = &data_0208e4b8;
        *(void **)p = &_ZTV5Stage;
        *(unsigned char *)(int)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 1;
        *(unsigned char *)(int)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 4;
        *(void **)p = &data_ov003_020b179c;
    }
    return p;
}
