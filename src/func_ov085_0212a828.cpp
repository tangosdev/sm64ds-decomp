//cpp
extern "C" {
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN6Player7TryGrabER5Actor(void* thiz, void* a);
extern void func_ov085_0212bc78(void* c, void* p);
extern int data_ov085_021306ac[];
extern int data_ov085_021306bc[];

void func_ov085_0212a828(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x134);
    if (id == 0) return;
    char* o = (char*)_ZN5Actor10FindWithIDEj(id);
    if (o == 0) return;
    int b = (*(unsigned short*)(o + 0xc) == 0xbf);
    if (b == 0) return;
    if ((*(int*)(c + 0x130) & 0x1000) == 0) return;
    if (_ZN6Player7TryGrabER5Actor(o, c) == 0) return;
    *(void**)(c + 0x45c) = o;
    *(int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFF) |= 2;
    if (*(unsigned char*)(c + 0x426) == 0) {
        func_ov085_0212bc78(c, data_ov085_021306ac);
    } else {
        func_ov085_0212bc78(c, data_ov085_021306bc);
    }
}
}
