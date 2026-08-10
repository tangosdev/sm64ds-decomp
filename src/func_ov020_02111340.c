// @symbol func_ov020_02111340
/* recovered: shared common types */
#include "common.h"
#define AT(p, off) ((void *)(int)((char *)(p) + (off)))
extern int RandomIntInternal(void *p);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *rot, int e, int f);
extern int data_0209e650[];



void func_ov020_02111340(char *c)
{
    int bit = ((unsigned int)RandomIntInternal(data_0209e650) >> 16) & 1;
    if (*(int *)(c + 0x41c) == 0)
        return;
    {
        char *found = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x418));
        if (found == 0)
            return;
        {
            struct Vector3 pos;
            struct Vector3_16 rot;
            int *fp = (int *)AT(found, 0x5c);
            int mul = 0x1f4 * bit;
            int x = fp[0];
            int dx = mul - 0xfa;
            pos.x = x;
            {
                int y = fp[1];
                int nx = x + (dx << 12);
                pos.y = y;
                {
                    int z = fp[2];
                    int ny = y + 0x41000;
                    pos.z = z;
                    pos.x = nx;
                    pos.y = ny;
                }
            }
            pos.z = *(int *)(*(char **)(c + 0x41c) + 0x64);
            rot.y = (short)((bit << 15) + 0x4000);
            rot.x = 0;
            rot.z = 0;
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x145, 0, &pos, &rot, *(signed char *)(c + 0xcc), -1);
        }
    }
}
