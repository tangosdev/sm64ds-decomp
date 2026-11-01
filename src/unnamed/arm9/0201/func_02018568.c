/* "LZ77", the tag in front of an LZ-compressed game file. */
extern int data_0208edf8[];
extern int func_0206e28c(int a, void *b, unsigned int n);

/* Whether the file bytes at a open with the "LZ77" tag (func_0206e28c
   compares 4 bytes, memcmp-style). The loaders then read the LZ header at
   a + 4 (func_02018ac4) and decompress from there. */
int func_02018568(int a) {
    return func_0206e28c(a, data_0208edf8, 4) == 0;
}
