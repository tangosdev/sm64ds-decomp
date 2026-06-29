/* func_ov098_02139070 at 0x02139070
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
 */
struct Vector3 { int x, y, z; };

extern void func_ov098_02138e08(char* c);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void* self, const struct Vector3* vec);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const struct Vector3* pos);
extern void func_ov098_02138b28(char* c, int i);

void func_ov098_02139070(char* self) {
    struct Vector3 vec;
    struct Vector3 vec2;
    int x, y, z;
    func_ov098_02138e08(self);
    x = *(int*)(self + 0x5c);
    y = *(int*)(self + 0x60) + 0x28000;
    z = *(int*)(self + 0x64);
    vec.x = x;
    vec.y = y;
    vec.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor19DisappearPoofDustAtERK7Vector3(self, &vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, (struct Vector3*)(self + 0x74));
    func_ov098_02138b28(self, 6);
}