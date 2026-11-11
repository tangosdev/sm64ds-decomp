/* "LZ77", the tag in front of an LZ-compressed game file. */
extern int data_0208edf8[];
/* local extern: the file address is an int here, not msl/string.h's void *. */
extern int memcmp(int a, void *b, unsigned int n);

/* Whether the file bytes at a open with the "LZ77" tag (memcmp of the
   first 4 bytes). The loaders then read the LZ header at
   a + 4 (func_02018ac4) and decompress from there. */
int func_02018568(int a) {
    return memcmp(a, data_0208edf8, 4) == 0;
}
