#include "types.h"
extern int _ZN4cstd6strcmpEPKcS1_(const char* a, const char* b);
extern void Crash(void);
typedef struct {
    const char* name;
    char pad[0xc];
} Entry16;

typedef struct {
    char pad[0x1c];
    u32 count;
    Entry16* entries;
} Table2;

int func_02047144(const Table2* tbl, const char* name)
{
    u32 count = tbl->count;
    u32 i = 0;
    if (i >= count)
        goto not_found;
    {
        Entry16* entries = tbl->entries;
        while (1)
        {
            if (_ZN4cstd6strcmpEPKcS1_(entries[i].name, name) == 0)
                return (int)i;
            i++;
            if (i >= tbl->count)
                break;
        }
    }
not_found:
    Crash();
    return -1;
}
