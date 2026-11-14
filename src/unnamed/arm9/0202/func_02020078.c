extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* daPly_c 0 (the parent) only: bits 0-1 of link settings byte 3 become
   flags. dScEntry_c::UpdateCourseSelect posts 0 for a touched course,
   1 or 2 for pressed bits 4 and 8 (SELECT, START), then raises the go
   flag (func_02020124); func_020200b8 reads it back on every console. */
void func_02020078(unsigned int flags) {
    if (_Z13func_0203da9cv() != 0)
        return;
    int v = _Z17GetPlayerFlagByteii(0, 3);
    _Z13func_0203db3cii(3, (unsigned char)(flags | (v & ~3)));
}
