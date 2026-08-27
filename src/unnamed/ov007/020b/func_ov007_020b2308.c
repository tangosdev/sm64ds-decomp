/* func_ov007_020b2308 at 0x020b2308
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void* func_ov007_020c162c(void* a0, int a1, int a2, int a3, int a4, unsigned short a5);

void* func_ov007_020b2308(void* a0, int a1)
{
    char* arr = *(char**)((char*)a0 + 0x18);
    char* entry = arr + a1 * 0x14;
    unsigned int bits = *(unsigned int*)(entry + 0x10);
    char* arr2 = *(char**)((char*)a0 + 0x20);
    char* e2 = arr2 + a1 * 0x10;
    return func_ov007_020c162c(
        (void*)((bits << 3) >> 29),
        *(int*)(entry + 4),
        (bits << 9) >> 29,
        (bits << 6) >> 29,
        *(int*)(e2 + 4),
        (unsigned short)(*(int*)(e2 + 8) / 2));
}
