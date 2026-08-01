//cpp
extern "C" {
int _ZN5Enemy11UpdateDeathER12WithMeshClsn(void* self, void* wm);
void _ZN9Animation7AdvanceEv(void* self);
char* _ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN5Enemy9SpawnCoinEv(void* self);
void func_ov084_02129498(char* r0);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
void func_02012694(int a, void* p);
void func_ov084_021296cc(char* c);
void _ZN8CapEnemy15RespawnIfHasCapEv(void* self);
void _ZN5Actor19UntrackInDeathTableEv(void* self);
extern int data_ov084_02130218[];

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

int func_ov084_021298d0(char* c){
    int r4 = _ZN5Enemy11UpdateDeathER12WithMeshClsn(c, c + 0x1b4);
    if ((unsigned int)(*(int*)(c + 0x10c) - 2) <= 4) {
        *(int*)(c + 0x3cc) = 0x1000;
        _ZN9Animation7AdvanceEv(c + 0x3c0);
        if (*(int*)(c + 0x460) == 3) {
            unsigned int id = *(unsigned int*)(c + 0x1a4);
            if (id != 0) {
                char* r = _ZN5Actor10FindWithIDEj(id);
                if (r != 0) {
                    int b = (int)(*(unsigned short*)(r + 0xc) == 0xc6);
                    if (b != 0) {
                        _ZN5Enemy9SpawnCoinEv(c);
                        func_ov084_02129498(c);
                    }
                }
            }
            *(int*)AT(c, 0x198) |= 0x20000;
            _ZN12CylinderClsn5ClearEv(c + 0x180);
            _ZN12CylinderClsn6UpdateEv(c + 0x180);
        }
    }
    if (r4 != 0) {
        func_02012694(data_ov084_02130218[*(int*)(c + 0x460)], c + 0x74);
        func_ov084_021296cc(c);
        if ((*(unsigned char*)(c + 0x113) & 0xf) < 6 || *(unsigned char*)(c + 0x464) == 2) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
            *(int*)(c + 0x60) = *(int*)(c + 0x420);
            *(int*)(c + 0x64) = *(int*)(c + 0x424);
            _ZN8CapEnemy15RespawnIfHasCapEv(c);
        }
        if ((*(unsigned char*)(c + 0x113) & 0xf) < 6) {
            _ZN5Actor19UntrackInDeathTableEv(c);
        }
    }
    return r4;
}
}
