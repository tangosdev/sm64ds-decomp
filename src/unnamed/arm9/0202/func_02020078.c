extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* daPly_c 0 (the parent) only: bits 0-1 of link settings byte 3 become
   flags. dScEntry_c::UpdateCourseSelect posts 0 for a touched course,
   1 or 2 for pressed bits 4 and 8 (SELECT, START), then raises the go
   flag (func_02020124); func_020200b8 reads it back on every console. */
void func_02020078(unsigned int flags) {
    if (func_0203da9c() != 0)
        return;
    int v = GetLinkByte(0, 3);
    SetLinkByte(3, (unsigned char)(flags | (v & ~3)));
}
