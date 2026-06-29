//cpp
extern "C" {
extern void *_ZN5Actor10FindWithIDEj(unsigned);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *thiz, const void *v, unsigned, int, unsigned, unsigned, unsigned);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
extern int func_ov098_0213b63c(char *c);

struct Vector3 { int x, y, z; };

int func_ov098_0213b6e0(char *c) {
    Vector3 vec;
    unsigned id = *(unsigned *)(c + 0x134);
    if (id != 0) {
        void *found = _ZN5Actor10FindWithIDEj(id);
        if (found != 0) {
            unsigned short k = *(unsigned short *)((char *)found + 0xc);
            int t = k == 0xbf;
            if (t) {
                vec.x = *(int *)(c + 0x5c);
                vec.y = *(int *)(c + 0x60);
                vec.z = *(int *)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &vec, 1, 0xc000, 1, 0, 1);
                if (*(int *)(c + 0x3c8) == 1) {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1c, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1d, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                    func_ov098_0213b63c(c);
                    return 1;
                }
            } else {
                t = k == 0xce;
                if (t) {
                    if (*(int *)(c + 0x3c8) == 1) {
                        func_ov098_0213b63c(c);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
}
