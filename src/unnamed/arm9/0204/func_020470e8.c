typedef struct Table Table;
typedef struct Entry { short val; char pad[2]; const char *name; char pad2[0x34]; } Entry;
typedef struct Header { char pad[8]; unsigned short count; char pad2[2]; Entry *entries; } Header;

extern int func_02046590(const Table *tbl, const char *name);

/* dExtAnmMaterial_c::Prepare's body (see dExtAnmMaterial_c.h): give each
   of the material-animation file's 0x3c-byte entries the index of the
   BMD material its name names (func_02046590). */
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
