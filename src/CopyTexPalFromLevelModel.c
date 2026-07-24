typedef unsigned int u32;

struct Tex { char* name; char pad[0x10]; };   /* stride 0x14 */
struct Pal { char* name; char pad[0xc]; };     /* stride 0x10 */

struct Entry {
    u32 texIdx;      /* 0x00 */
    u32 palIdx;      /* 0x04 */
    char pad1[0x14]; /* 0x08 */
    u32 f1c;         /* 0x1c */
    u32 f20;         /* 0x20 */
    char pad2[0xc];  /* 0x24 */
};                   /* 0x30 */

struct Model {
    char pad0[0x18];
    struct Tex* texArr;   /* 0x18 */
    char pad1[4];         /* 0x1c */
    struct Pal* palArr;   /* 0x20 */
    u32 count;            /* 0x24 */
};

struct Arg {
    struct Model* model;   /* 0x00 */
    struct Entry* entries; /* 0x04 */
};

struct Mgr {
    struct Model* model;   /* 0x00 */
    struct Entry* entries; /* 0x04 */
};

extern struct Mgr* data_0209f320;
extern int _ZN4cstd6strcmpEPKcS1_(const char*, const char*);

int CopyTexPalFromLevelModel(struct Arg* arg)
{
    int ret = 1;
    struct Entry* sbase = data_0209f320->entries;
    struct Model* a = arg->model;
    struct Tex* atex = a->texArr;
    struct Model* sm = data_0209f320->model;
    struct Tex* mtex = sm->texArr;
    struct Pal* mpal = sm->palArr;
    struct Pal* apal = a->palArr;
    struct Entry* e = arg->entries;
    u32 i;

    for (i = 0; i < a->count; i++) {
        char* texName = atex[e->texIdx].name;
        char* palName = apal[e->palIdx].name;
        struct Entry* s = sbase;
        int found = 0;
        u32 j;
        for (j = 0; j < sm->count; j++) {
            if (_ZN4cstd6strcmpEPKcS1_(texName, mtex[s->texIdx].name) == 0 &&
                _ZN4cstd6strcmpEPKcS1_(palName, mpal[s->palIdx].name) == 0) {
                e->f1c = s->f1c;
                e->f20 = s->f20;
                found = 1;
            }
            s++;
        }
        if (!found) ret = 0;
        e++;
    }
    return ret;
}
