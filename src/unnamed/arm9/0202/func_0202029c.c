extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* daPly_c 0 only: bits 0-2 of link settings byte 1 become flags.
   dScEntry_c::UpdateCourseSelect posts the chosen course index
   (data_0209b2e4); the other consoles copy func_020202dc into it. */
void func_0202029c(unsigned int flags) {
    if (func_0203da9c() != 0)
        return;
    int v = GetPlayerFlagByte(0, 1);
    func_0203db3c(1, (unsigned char)(flags | (v & ~7)));
}
