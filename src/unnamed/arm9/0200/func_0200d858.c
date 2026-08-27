struct Camera;
struct Vector3;
extern void AddVec3(struct Vector3 *dst, const struct Vector3 *src, struct Vector3 *dst2);

void func_0200d858(struct Camera *self, const struct Vector3 *offset)
{
    AddVec3((struct Vector3 *)((char *)self + 0x80), offset, (struct Vector3 *)((char *)self + 0x80));
    AddVec3((struct Vector3 *)((char *)self + 0x8c), offset, (struct Vector3 *)((char *)self + 0x8c));
}
