//cpp
extern "C" {
int func_ov002_020b5e58(void* c, void* d);
int _ZN5Actor17GetWaterHeightWDWEv(void* c);
void _ZN11RaycastLineC1Ev(void*);
void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void*, void*, void*, void*);
int _ZN11RaycastLine10DetectClsnEv(void*);
void _ZN11RaycastLine10GetClsnPosEv(void*, void*);
void _ZN11RaycastLineD1Ev(void*);
extern int data_ov029_02113f00[];

int func_ov029_02111f58(char* c)
{
    int sppad[6];
    int pos[3];
    char rl[0x7c];
    int wh;
    int x, y, z;

    if (func_ov002_020b5e58(c, data_ov029_02113f00) != 0) {
        wh = _ZN5Actor17GetWaterHeightWDWEv(c);
        if (*(int*)(c + 0x60) > wh) {
            _ZN11RaycastLineC1Ev(rl);
            x = *(int*)(c + 0x5c);
            sppad[3] = x;
            y = *(int*)(c + 0x60);
            sppad[4] = y;
            z = *(int*)(c + 0x64);
            sppad[1] = y;
            sppad[5] = z;
            sppad[0] = x;
            sppad[2] = z;
            sppad[1] = y + 0x14000;
            sppad[4] = wh;
            _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &sppad[0], &sppad[3], c);
            if (_ZN11RaycastLine10DetectClsnEv(rl) == 0) {
                *(int*)(c + 0x60) = wh;
            } else {
                _ZN11RaycastLine10GetClsnPosEv(pos, rl);
                *(int*)(c + 0x60) = pos[1];
            }
            _ZN11RaycastLineD1Ev(rl);
        }
        *(int*)(c + 0x320) = *(int*)(c + 0x5c);
        *(int*)(c + 0x324) = *(int*)(c + 0x60);
        *(int*)(c + 0x328) = *(int*)(c + 0x64);
        return 1;
    }
    return 0;
}
}
