typedef struct { int x, y, z; } Vector3;

extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* prev);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* v, void* w, int e, int f);
extern void _ZN9PowerStar13AddStarMarkerEv(void* thiz);

void func_ov002_020f05f4(char* c)
{
    char* a = 0;
    for (;;) {
        a = (char*)_ZN5Actor15FindWithActorIDEjPS_(0xb4, a);
        if (a == 0) return;
        if (*(unsigned char*)(c + 0x10d) == *(unsigned char*)(a + 0x1d9)) {
            int* base = (int*)(a + 0x5c);
            Vector3 pos;
            pos.x = base[0];
            pos.y = base[1];
            pos.z = base[2];
            pos.y += 0x12c000;
            {
                char* p = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, (*(unsigned char*)(c + 0x10d)) | 0x40, &pos, 0, *(signed char*)(c + 0xcc), -1);
                if (p != 0) {
                    _ZN9PowerStar13AddStarMarkerEv(p);
                }
            }
            return;
        }
    }
}
