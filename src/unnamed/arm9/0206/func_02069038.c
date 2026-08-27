extern void func_02059f2c(void *p);

int func_02069038(void) {
    unsigned char buf[12];
    int i;
    int sum;
    func_02059f2c(buf);
    for (i = 0, sum = 0; i < 6; i++) {
        sum += buf[i];
    }
    {
        unsigned int n = (sum + *(int *)0x27ffc3c) * 7;
        return n % 20;
    }
}
