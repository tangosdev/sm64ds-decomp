typedef long long s64;
typedef struct { int x, y, z; } Vector3;

extern int func_ov074_021223bc(char *c);
extern void func_ov074_0212199c(char *c);
extern void func_ov074_0212042c(char *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *thiz, void *clsn, unsigned int a);
extern void func_ov074_02120d74(char *c);
extern void func_ov074_02121380(char *c);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void _ZN5Actor13LandingDustAtER7Vector3b(void *thiz, Vector3 *v, int b);
extern void func_02012694(int a, void *p);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *thiz, Vector3 *v, int f);
extern void func_ov074_02121300(char *c);
extern void func_ov074_0212195c(char *c);
extern void func_ov074_02121800(char *c);

extern char *data_0209f318;
extern int data_ov074_0212290c[];

int _ZN8Goomboss8BehaviorEv(char *c)
{
    int t;
    int u;
    int quake;

    if (*(int *)(c + 8) == 0x1111) {
        return func_ov074_021223bc(c);
    }

    t = ((s64)*(short *)(c + 0x5f6) * 0x646 + 0x800) >> 12;
    u = ((s64)t * 0x546000 + 0x800) >> 12;
    *(int *)(c + 0x5e8) = u < 0 ? -u : u;

    if (*(int *)(c + 0x5cc) != 7) {
        *(void **)(data_0209f318 + 0x114) = c;
    }

    func_ov074_0212199c(c);
    func_ov074_0212042c(c);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x40c, 0);
    func_ov074_02120d74(c);
    func_ov074_02121380(c);

    {
        int i;
        char *p = c + 0x110;
        for (i = 0; i < 4; i++) {
            _ZN12CylinderClsn5ClearEv(p);
            _ZN12CylinderClsn6UpdateEv(p);
            p += 0x40;
        }
    }

    if (*(int *)(c + 0x5cc) != 1 && *(int *)(c + 0x5cc) != 0) {
        quake = data_ov074_0212290c[*(unsigned char *)(c + 0x604)];

        if (*(unsigned char *)(c + 0x5fe) != 0) {
            if (*(unsigned char *)(c + 0x604) >= 2) {
                Vector3 v;
                v.x = *(int *)(c + 0x3c4);
                v.y = *(int *)(c + 0x3c8);
                v.z = *(int *)(c + 0x3cc);
                _ZN5Actor17HugeLandingDustAtER7Vector3b(c, &v, 1);
            } else {
                Vector3 v;
                v.x = *(int *)(c + 0x3c4);
                v.y = *(int *)(c + 0x3c8);
                v.z = *(int *)(c + 0x3cc);
                _ZN5Actor13LandingDustAtER7Vector3b(c, &v, 1);
            }
            func_02012694(0x15e, c + 0x74);
            {
                Vector3 v;
                v.x = *(int *)(c + 0x3c4);
                v.y = *(int *)(c + 0x3c8);
                v.z = *(int *)(c + 0x3cc);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, quake);
            }
        }

        if (*(unsigned char *)(c + 0x5ff) != 0) {
            if (*(unsigned char *)(c + 0x604) >= 2) {
                Vector3 v;
                v.x = *(int *)(c + 0x3b8);
                v.y = *(int *)(c + 0x3bc);
                v.z = *(int *)(c + 0x3c0);
                _ZN5Actor17HugeLandingDustAtER7Vector3b(c, &v, 1);
            } else {
                Vector3 v;
                v.x = *(int *)(c + 0x3b8);
                v.y = *(int *)(c + 0x3bc);
                v.z = *(int *)(c + 0x3c0);
                _ZN5Actor13LandingDustAtER7Vector3b(c, &v, 1);
            }
            func_02012694(0x15e, c + 0x74);
            {
                Vector3 v;
                v.x = *(int *)(c + 0x3b8);
                v.y = *(int *)(c + 0x3bc);
                v.z = *(int *)(c + 0x3c0);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, quake);
            }
        }
    }

    func_ov074_02121300(c);
    func_ov074_0212195c(c);
    func_ov074_02121800(c);
    return 1;
}
