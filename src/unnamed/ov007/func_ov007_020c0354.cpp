//cpp
// Each screen's tilemap buffer is cleared in eight-byte blocks.
extern "C" void func_ov007_020c0354(int (*words)[2], unsigned count) {
    for (unsigned i = 0; i < count; i++) {
        words[i][1] = 0;
        words[i][0] = words[i][1];
    }
}
