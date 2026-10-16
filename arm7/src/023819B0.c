/* storage 0x023819B0 runtime 0x037F9848 size 0x14 wram ARM.
 * Indexed table read. V3: pointer-stepped base. */

int fn_023819B0(int a1) {
    char *p = (char *)0x2700000 + (a1 << 2);
    p += 0xFF000;
    return *(int *)(p + 0xDA0);
}
