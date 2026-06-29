/* func_ov060_02118970 at 0x02118970
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov060).
 */
enum Bool { FALSE, TRUE };

struct Vector3 { int x, y, z; };

extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void func_02012694(int a, void* b);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, struct Vector3* v, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, struct Vector3* v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
extern void func_ov060_021184bc(char* c);

void func_ov060_02118970(char* c)
{
    char* a;
    struct Vector3 v1, v2;
    enum Bool isType;
    unsigned int id;
    id = *(unsigned int*)(c + 0x148);
    if (id == 0) return;
    a = _ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    isType = (enum Bool)(*(unsigned short*)(a + 0xc) == 0xbf);
    if (!isType) return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa8, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa9, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xaa, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xab, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xac, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    func_02012694(0x2f, c + 0x74);
    v1.x = *(int*)(c + 0x5c);
    v1.y = *(int*)(c + 0x60);
    v1.z = *(int*)(c + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v1, 0x7d0000);
    v2.x = *(int*)(c + 0x5c);
    v2.y = *(int*)(c + 0x60);
    v2.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v2, 2, 0xc000, 1, 0, 1);
    func_ov060_021184bc(c);
}
