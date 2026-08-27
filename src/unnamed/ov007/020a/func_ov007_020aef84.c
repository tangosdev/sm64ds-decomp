extern void func_ov007_020c1d8c(int i);
extern void RequestScreenFade(int r0, int r1, int r2);
extern int func_ov007_020b0520;

void func_ov007_020aef84(void) {
    func_ov007_020c1d8c(0);
    RequestScreenFade(2, 1, (int)&func_ov007_020b0520);
}
