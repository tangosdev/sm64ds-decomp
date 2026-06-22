extern void func_0205b504(int i);
extern void func_0205a86c(int a, int b, int c, int d, int e);

void func_0205abb8(int a, int b, int c, int d)
{
    int i;
    unsigned int mask = (unsigned int)c;
    for (i = 0; i < 8 && mask != 0; ) {
        if (mask & 1) {
            func_0205b504(i);
        }
        i++;
        mask >>= 1;
    }
    func_0205a86c(0xa, a, b, c, d);
}
