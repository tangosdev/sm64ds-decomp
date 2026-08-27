//cpp
struct MemoryNode {
    unsigned short pad0;
    unsigned short bits2;
    int f4;
    char rest[8];
};
struct Target {
    char *p0;
    int f4;
};
extern "C" void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct Target *thiz, struct MemoryNode *node);
extern "C" void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct Target *thiz, struct MemoryNode *node) {
    thiz->p0 = (char *)node - (unsigned short)((node->bits2 >> 8) & 0x7f);
    thiz->f4 = node->f4 + (int)((char *)node + 0x10);
}
