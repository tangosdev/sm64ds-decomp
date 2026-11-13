extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* daPly_c 0 only: bits 0-2 of link settings byte 1 become flags.
   dScEntry_c::UpdateCourseSelect posts the chosen course index
   (data_0209b2e4); the other consoles copy func_020202dc into it. */
void func_0202029c(unsigned int flags) {
    if (_Z13func_0203da9cv() != 0)
        return;
    int v = _Z17GetPlayerFlagByteii(0, 1);
    _Z13func_0203db3cii(1, (unsigned char)(flags | (v & ~7)));
}
