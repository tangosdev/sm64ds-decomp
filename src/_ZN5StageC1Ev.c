#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off)))))
extern int _ZN9ActorBasenwEj(unsigned int);
extern int _ZN9ActorBaseC1Ev(void *);
extern int _ZN8Particle10SysTrackerC1Ev(void *);
extern int _ZN5ModelC1Ev(void *);
extern int _ZN12MeshColliderC1Ev(void *);
extern int data_0208e4b8[];
extern int _ZTV5Scene[];
extern int _ZTV5Stage[];

void *_ZN5StageC3Ev(void)
{
    char *p = (char*)_ZN9ActorBasenwEj(0x9c8);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(int*)(p) = (int)data_0208e4b8;
        *(int*)(p) = (int)_ZTV5Scene;
        *(unsigned char*)AT(p, 0x13) |= 1;
        *(unsigned char*)AT(p, 0x13) |= 4;
        *(int*)(p) = (int)_ZTV5Stage;
        _ZN8Particle10SysTrackerC1Ev(p+0x50);
        _ZN5ModelC1Ev(p+0x86c);
        _ZN12MeshColliderC1Ev(p+0x91c);
    }
    return p;
}
