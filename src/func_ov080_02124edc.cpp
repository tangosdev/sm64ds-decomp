//cpp
struct Vector3 { int x, y, z; };

extern "C" {
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const Vector3& v, unsigned n, int f, short s);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
void _ZN12CylinderClsn5ClearEv(void* self);
}

extern "C" int func_ov080_02124edc(char* c);
int func_ov080_02124edc(char* c)
{
    *(short*)(c + 0x8c) = *(short*)(*(char**)(c + 0x374) + 0x8c);
    *(short*)(c + 0x8e) = *(short*)(*(char**)(c + 0x374) + 0x8e);
    *(short*)(c + 0x92) = *(short*)(c + 0x8c);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    {
        int f = *(int*)(c + 0xb0);
        int a = (int)((f & 0x100) != 0);
        if (a != 0) {
            int b = (int)((f & 0x2000) != 0);
            if (b == 0) goto clear;
        }
        {
            Vector3 vec;
            Vector3 vec2;
            int x = *(int*)(c + 0x5c);
            int z = *(int*)(c + 0x64);
            int y = *(int*)(c + 0x60) + 0xb4000;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            ((int*)&vec2)[0] = ((int*)&vec)[0];
            ((int*)&vec2)[1] = ((int*)&vec)[1];
            ((int*)&vec2)[2] = ((int*)&vec)[2];
            _ZN5Actor10PoofDustAtERK7Vector3(c, vec2);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);

            {
                int ybase = *(int*)(c + 0x60);
                int xx = vec.x;
                int y2 = ybase + 0x64000;
                int zz = vec.z;
                Vector3 v3;
                v3.x = xx;
                v3.z = zz;
                vec.y = y2;
                v3.y = y2;
                _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v3, 5, 0xf000, 0);
            }

            _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
    }
clear:
    _ZN12CylinderClsn5ClearEv(c + 0x14c);
    return 1;
}
