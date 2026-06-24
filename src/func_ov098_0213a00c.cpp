//cpp
typedef short s16;
struct Vector3;
struct Actor;
extern "C" {
Actor* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, Actor* after);
s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int Vec3_Dist(const Vector3* a, const Vector3* b);
int AngleDiff(int a, int b);
void func_ov098_0213a00c(char *c);
}
void func_ov098_0213a00c(char *c)
{
    if (*(unsigned char*)(c+0x341) != 0) return;
    Actor *r = _ZN5Actor15FindWithActorIDEjPS_(*(unsigned short*)(c+0xc), 0);
    while (r) {
        if (r != (Actor*)c) {
            Vector3 *b = (Vector3*)((char*)r+0x5c);
            Vector3 *a = (Vector3*)(c+0x5c);
            int ang = Vec3_HorzAngle(a, b);
            if (Vec3_Dist(a, b) < 0x96000) {
                if (AngleDiff(ang, *(s16*)(c+0x8e)) < 0x4000)
                    *(Actor**)(c+0x348) = r;
                else
                    *(Actor**)(c+0x344) = r;
            }
        }
        r = _ZN5Actor15FindWithActorIDEjPS_(*(unsigned short*)(c+0xc), r);
    }
    *(unsigned char*)(c+0x341) = 1;
}
