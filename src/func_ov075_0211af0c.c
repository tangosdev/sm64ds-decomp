typedef int Fix12i;
struct Vector3 { int x, y, z; };
extern Fix12i Vec3_Dist(const struct Vector3* a, const struct Vector3* b);

struct Elem { struct Vector3 pos; int dist; int pad[2]; };  /* 0x18 */

void func_ov075_0211af0c(char* c)
{
    int i;
    struct Vector3 tmp;
    for (i = 0; i < *(unsigned short*)(c + 0xa4); i++) {
        struct Elem* e = (struct Elem*)(*(char**)(c + 0x80) + i * 0x18);
        tmp = e->pos;
        tmp.z = 0;
        e->dist = Vec3_Dist((struct Vector3*)(c + 0x74), &tmp);
    }
}
