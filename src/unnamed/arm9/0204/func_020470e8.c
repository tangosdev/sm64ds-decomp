typedef struct Table Table;
typedef struct Entry { short val; char pad[2]; const char *name; char pad2[0x34]; } Entry;
typedef struct Header { char pad[8]; unsigned short count; char pad2[2]; Entry *entries; } Header;

extern short func_02046590(const Table *tbl, const char *name);

void func_020470e8(const Table *tbl, Header *h)
{
    int i = 0;
    Entry *e = h->entries;
    while (i < h->count) {
        e->val = func_02046590(tbl, e->name);
        i++;
        e++;
    }
}
