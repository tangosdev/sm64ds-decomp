extern void func_0205a86c(int a, unsigned b, int c, unsigned d, unsigned e);

void func_0205aa10(unsigned p0, unsigned p1, int p2, unsigned p3,
                   unsigned p4, unsigned p5, unsigned p6, unsigned p7,
                   unsigned p8, unsigned p9) {
    func_0205a86c(0xb,
                  p0 | (p8 << 16),
                  p2,
                  p5 | ((p6 << 24) | (p7 << 22)),
                  p4 | ((p3 << 26) | (p1 << 24) | (p9 << 16)));
}
