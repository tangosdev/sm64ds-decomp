#include "types.h"
extern int _ZN4cstd6strcmpEPKcS1_(const char* a, const char* b);
extern void Crash(void);
typedef struct {
    const char* name;
    char pad[0x2c];
} Entry;

typedef struct {
    char pad[0x14];
    u32 count;
    Entry* entries;
} Table;

int func_020471ac(const Table* tbl, const char* name)
{
    u32 count = tbl->count;
    u32 i = 0;
    if (i >= count)
        goto not_found;
    {
        Entry* entry = tbl->entries;
        while (1)
        {
            if (_ZN4cstd6strcmpEPKcS1_(entry->name, name) == 0)
                return (int)i;
            i++;
            entry = (Entry*)((char*)entry + 0x14);
            if (i >= tbl->count)
                break;
        }
    }
not_found:
    Crash();
    return -1;
}
