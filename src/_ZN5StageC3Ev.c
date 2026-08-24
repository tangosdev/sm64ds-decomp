#define AT(p, off) ((void *)(int)((char *)(p) + (off)))
extern int _ZN7fBase_cnwEj(unsigned int);
extern int _ZN7fBase_cC2Ev(void *);
extern int _ZN8Particle10SysTrackerC1Ev(void *);
extern int _ZN5ModelC1Ev(void *);
extern int _ZN7dBgW_KcC1Ev(void *);
extern int data_0208e4b8[];
extern int _ZTV8dScene_c[];
extern int _ZTV5Stage[];

void *_ZN5StageC3Ev(void)
{
    char *p = (char*)_ZN7fBase_cnwEj(0x9c8);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(int*)(p) = (int)data_0208e4b8;
        *(int*)(p) = (int)_ZTV8dScene_c;
        *(unsigned char*)AT(p, 0x13) |= 1;
        *(unsigned char*)AT(p, 0x13) |= 4;
        *(int*)(p) = (int)_ZTV5Stage;
        _ZN8Particle10SysTrackerC1Ev(p+0x50);
        _ZN5ModelC1Ev(p+0x86c);
        _ZN7dBgW_KcC1Ev(p+0x91c);
    }
    return p;
}
