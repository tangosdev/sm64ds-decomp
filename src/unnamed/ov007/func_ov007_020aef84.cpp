//cpp


extern "C" void func_ov007_020c1d8c(int i);
extern "C" void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern "C" void func_ov007_020b0520(void);

void func_ov007_020aef84(void) {
    func_ov007_020c1d8c(0);
    RequestScreenFade(2, 1, func_ov007_020b0520);
}

