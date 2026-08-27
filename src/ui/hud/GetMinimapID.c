typedef signed char s8;

struct Node {
    int f0;
    s8 f4;
    char pad5[7];
    struct Node *fc;
};

struct Entry {
    char pad0[8];
    struct Node *f8;
};

struct Obj {
    char pad0[0x60];
    int f60;
    char pad64[0xcc - 0x64];
    s8 fcc;
};

extern char *data_0209f314;
extern s8 data_02092120;
extern char *data_0209f340;

int GetMinimapID(struct Obj *obj, int arg)
{
    int idx = obj->fcc;
    char *g;
    int val;
    struct Node *node;
    if (idx < 0)
        idx = data_02092120;
    g = data_0209f314;
    if (g == 0)
        goto tail;
    node = ((struct Entry *)(g + idx * 0xc))->f8;
    if (node == 0)
        goto tail;
    val = obj->f60;
    if (node == 0)
        goto L74;

    do {
        int d = val - node->f0;
        if (d <= 0)
            goto L68;
        if (arg < 0)
            goto L74;
        if (d >= 0xc8000)
            goto L74;
        return arg;
    L68:
        node = node->fc;
    } while (node != 0);
L74:
    if (node != 0)
        return node->f4;
tail:
    return *(s8 *)(*(char **)(data_0209f340 + 0x10) + idx * 0xc + 8);
}
