/* func_ov003_020adc10 at 0x020adc10 (ov003), size 0x64
 * Compiler mwccarm 1.2/sp2p3 */
extern int data_0208e4b8;
extern int _ZTV5Stage;
extern int data_ov003_020b1650;
extern void *_ZN9ActorBasenwEj(unsigned int);
extern void _ZN9ActorBaseC1Ev(void *);

void *func_ov003_020adc10(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x54);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(int **)p = &data_0208e4b8;
        *(int **)p = &_ZTV5Stage;
        *(unsigned char *)(int)(((long long)(int)(p + 0x13)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        *(unsigned char *)(int)(((long long)(int)(p + 0x13)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
        *(int **)p = &data_ov003_020b1650;
    }
    return p;
}
