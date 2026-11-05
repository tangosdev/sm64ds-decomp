extern int data_020a4c5c[2];
extern int data_020a4c54[2];
extern int data_020a4c4c[2];

/* Reset both track volume ramps (see func_020490b0): current and target
   at 127 (0x7f000) and no step. func_02011dcc calls it once it starts a
   sequence on sMusicHandle, as Sound.cpp's music loaders (e.g.
   Sound::LoadAndSetMusic_Layer3) do. */
void func_02049764(void) {
    data_020a4c5c[0] = 0x7f000;
    data_020a4c54[0] = 0x7f000;
    data_020a4c4c[0] = 0;
    data_020a4c5c[1] = 0x7f000;
    data_020a4c54[1] = 0x7f000;
    data_020a4c4c[1] = 0;
}
