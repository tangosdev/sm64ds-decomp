/* storage 0x02398660 runtime 0x027CA5D8 size 0x20 + pool 0x28 main ARM.
 * Clamp result down by the table half when at/above the limit. */
extern int dword_380FFF4[];
unsigned fn_02398660(unsigned result) {
    if (result >= 0x4805F60)
        result -= *(unsigned short *)((char *)*(int **)dword_380FFF4 + 918);
    return result;
}
