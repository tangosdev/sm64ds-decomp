extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(void *self, void *a, unsigned int b, void *c, unsigned int d);

void func_ov021_02112294(void *c)
{
    if (*(unsigned char*)((char*)c + 0x3c1) == 0xff)
        return;
    if (*(unsigned char*)((char*)c + 0x3be) == 4) {
        _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(c, (char*)c + 0x3c0, *(unsigned char*)((char*)c + 0x3c1), (char*)c + 0x5c, 2);
    } else {
        _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(c, (char*)c + 0x3c0, *(unsigned char*)((char*)c + 0x3c1), (char*)c + 0x5c, 4);
    }
}
