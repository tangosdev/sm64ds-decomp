typedef short s16;
typedef int Fix12i;

struct Vector3 {
    Fix12i x;
    Fix12i y;
    Fix12i z;
};

struct dActor_c {
    unsigned char pad0[0x80];
    struct Vector3 scale; /* @0x80 */
};

extern s16 ReadUnalignedShort(const char *from);

int func_02008500(struct dActor_c *actor, const char *data) {
    Fix12i x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    actor->scale.x = x;
    actor->scale.y = y;
    actor->scale.z = z;
    return 1;
}
