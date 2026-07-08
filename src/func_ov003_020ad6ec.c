extern int SELECTED_LINE[];
extern int LINE_SCROLL_POSITION[];
extern int build_time_file[];
extern void func_ov003_020adcbc(int a, int b, int c);
extern void func_ov004_020adc74(void* p, int b, int c);

void func_ov003_020ad6ec(void)
{
    int v = SELECTED_LINE[0];
    int nv = v - 0xa;
    LINE_SCROLL_POSITION[0] = nv;
    unsigned int t = (unsigned int)(v + 0xb);
    if (t > 0x36) {
        LINE_SCROLL_POSITION[0] = nv - (t - 0x36);
    }
    if (LINE_SCROLL_POSITION[0] < 0) LINE_SCROLL_POSITION[0] = 0;
    int s = LINE_SCROLL_POSITION[0];
    int cnt = 0x36 - s;
    int end = cnt + 2;
    if (end > 0x17) end = 0x17;
    int i;
    int j = s;
    for (i = 2; i < end; i++) {
        func_ov003_020adcbc(j, i, 3);
        j++;
    }
    func_ov004_020adc74(build_time_file, 0, 0);
}
