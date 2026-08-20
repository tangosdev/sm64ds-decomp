struct Vector3;
struct dActor_c;

extern void func_020374d4(void *d, struct Vector3 *s);
extern void func_020353b0(void *c, struct dActor_c *p);

void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *this_, struct Vector3 *vec, struct dActor_c *actor)
{
    func_020374d4(this_, vec);
    func_020353b0(this_, actor);
}
