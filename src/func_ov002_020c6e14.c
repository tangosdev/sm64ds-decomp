/* func_ov002_020c6e14 at 0x020c6e14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern unsigned char data_0209f2ac;
extern short data_ov002_020ff1a0[];
extern void* data_0209f318[];

extern int NumStars(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const void* pos, unsigned int a, unsigned int b);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);

int func_ov002_020c6e14(void* actor)
{
    int idx;

    if (data_0209f2ac != 0) {
        idx = -1;
        switch (NumStars()) {
        case 1:  idx = 0; break;
        case 3:  idx = 1; break;
        case 8:  idx = 2; break;
        case 12: idx = 3; break;
        case 30: idx = 4; break;
        case 50: idx = 5; break;
        case 80: idx = 6; break;
        }

        if (idx == -1)
            return 0;

        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(actor, actor, data_ov002_020ff1a0[idx], 0, 0, 2)) {
            _ZN6Camera9SetFlag_3Ev(data_0209f318[0]);
            return 1;
        }
    }

    return 0;
}