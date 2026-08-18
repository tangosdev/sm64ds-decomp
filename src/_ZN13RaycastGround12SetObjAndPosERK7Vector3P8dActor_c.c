struct Vector3;
struct Actor;

extern void func_020374d4(void *d, struct Vector3 *s);
extern void func_020353b0(void *c, struct Actor *p);

void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *this_, struct Vector3 *vec, struct Actor *actor)
{
    func_020374d4(this_, vec);
    func_020353b0(this_, actor);
}
