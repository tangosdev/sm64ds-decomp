extern void func_ov002_020bd984(char* c, unsigned int r1);
extern unsigned char CURRENT_GAMEMODE;
extern int SAVE_DATA;

void func_ov002_020e032c(unsigned char* c)
{
    unsigned char st;
    if (c[0x6ff] == 0) return;
    c[0x6ff] = 0;
    st = CURRENT_GAMEMODE;
    if ((int)(st == 1) == 0) {
        if (((&SAVE_DATA)[2] & 0x80) == 0) return;
        if ((int)(st == 2) != 0) return;
    }
    func_ov002_020bd984((char*)c, 0x33);
}
