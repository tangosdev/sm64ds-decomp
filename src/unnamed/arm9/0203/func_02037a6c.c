typedef int s32;

struct AABB {
    char pad[0x58];
    s32 minX, minY, minZ;   /* 0x58 0x5c 0x60 */
    s32 maxX, maxY, maxZ;   /* 0x64 0x68 0x6c */
};

void func_02037a6c(struct AABB *b, s32 x1, s32 y1, s32 z1, s32 x2, s32 y2, s32 z2)
{
    if (b->minX > x1) b->minX = x1;
    if (b->maxX < x1) b->maxX = x1;
    if (b->minX > x2) b->minX = x2;
    if (b->maxX < x2) b->maxX = x2;
    if (b->minY > y1) b->minY = y1;
    if (b->maxY < y1) b->maxY = y1;
    if (b->minY > y2) b->minY = y2;
    if (b->maxY < y2) b->maxY = y2;
    if (b->minZ > z1) b->minZ = z1;
    if (b->maxZ < z1) b->maxZ = z1;
    if (b->minZ > z2) b->minZ = z2;
    if (b->maxZ < z2) b->maxZ = z2;
}
