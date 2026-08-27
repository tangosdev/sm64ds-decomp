extern int func_02050a84(unsigned int id);
extern int func_0205179c(unsigned int id, int flags, int arg2);
extern int func_020518dc(void *a, void *b);
extern int func_020516d4(unsigned int id, int flags, int arg2);
extern int func_020518a0(void *a, void *b);

struct Entry {
    unsigned char kind;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
    int w;
};

struct List {
    unsigned int count;
    struct Entry entries[1];
};

int func_02051918(unsigned int id, int arg)
{
    struct List *l;
    unsigned int i;
    struct Entry *e;

    l = (struct List *)func_02050a84(id);
    if (!l)
        return 0;

    i = 0;
    if (i < l->count) {
        e = l->entries;
        do {
            switch (e->kind) {
            case 0:
                if (func_0205179c(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 3:
                if (func_020518dc((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            case 1:
                if (func_020516d4(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 2:
                if (func_020518a0((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            }
            i++;
            e++;
        } while (i < l->count);
    }

    return 1;
}
