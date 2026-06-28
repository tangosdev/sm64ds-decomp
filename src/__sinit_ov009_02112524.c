extern void func_02017acc(void*, int);
extern void func_02017b4c(void*, int);
extern void func_020731dc(void*, void*, void*);
extern int func_02017ab4[];
extern int func_02017b34[];
extern int func_020072c0[];
extern int data_ov009_02113c68[];
extern int data_ov009_02113c70[];
extern int data_ov009_02113c78[];
extern int data_ov009_02113c84[];
extern int data_ov009_02113c90[];
extern int data_ov009_02113c9c[];
extern int data_ov009_02113ca8[];
extern int data_ov009_02113cb4[];
extern int data_ov009_02113cc0[];
extern int data_ov009_02113ccc[];
extern int data_ov009_02113cd8[];
extern int data_ov009_02113ce4[];
extern int data_ov009_02113cf0[];
extern int data_ov009_02113cfc[];
extern int data_ov009_02113d08[];
extern int data_ov009_02113d14[];
extern int data_ov009_02113d20[];
extern int data_ov009_02113d2c[];
extern int data_ov009_02113d38[];
extern int data_ov009_02113d44[];
extern int data_ov009_02113d50[];
extern int data_ov009_02113d5c[];
extern int data_ov009_02113d68[];
extern int data_ov009_02113d74[];
extern int data_ov009_02113d80[];
extern int data_ov009_02113d8c[];
extern int data_ov009_02113d98[];
extern int data_ov009_02113da4[];
extern int data_ov009_02113db0[];
extern int data_ov009_02113dbc[];
extern int data_ov009_02113dc8[];
extern int data_ov009_02113dd4[];
extern int data_ov009_02113de0[];
extern int data_ov009_02113dec[];
extern int data_ov009_02113df8[];
extern int data_ov009_02113e04[];
extern int data_ov009_02113e10[];
extern int data_ov009_02113e1c[];
extern int data_ov009_02113e28[];
extern int data_ov009_02113e34[];
extern int data_ov009_02113e40[];
extern int data_ov009_02113e4c[];
extern int data_ov009_02113e58[];
extern int data_ov009_02113e64[];
extern int data_ov009_02113e70[];
extern int data_ov009_02113e7c[];

void __sinit_ov009_02112524(void)
{
    func_02017acc(data_ov009_02113c68, 0x693);
    func_020731dc(data_ov009_02113c68, func_02017ab4, data_ov009_02113c78);
    func_02017b4c(data_ov009_02113c70, 0x694);
    func_020731dc(data_ov009_02113c70, func_02017b34, data_ov009_02113cc0);
    *(unsigned int*)data_ov009_02113d8c = 0xfe94e000;
    *(unsigned int*)((char*)data_ov009_02113d8c + 4) = (unsigned int)-0x5a000;
    *(unsigned int*)((char*)data_ov009_02113d8c + 8) = 0xff60a000;
    func_020731dc(data_ov009_02113d8c, func_020072c0, data_ov009_02113d74);
    {
        unsigned int *p = (unsigned int*)data_ov009_02113d98;
        p[0] = 0xfea20000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xff52e000;
        func_020731dc(p, func_020072c0, data_ov009_02113d80);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113da4;
        p[0] = (unsigned int)-0x1540000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xff43e000;
        func_020731dc(p, func_020072c0, data_ov009_02113d08);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113db0;
        p[0] = 0xfeb56000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xff344000;
        func_020731dc(p, func_020072c0, data_ov009_02113d14);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113dbc;
        p[0] = 0xfebb0000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xff236000;
        func_020731dc(p, func_020072c0, data_ov009_02113d5c);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113dc8;
        p[0] = 0xfebe2000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xff114000;
        func_020731dc(p, func_020072c0, data_ov009_02113d50);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113dd4;
        p[0] = 0xfebe2000;
        p[1] = (unsigned int)-0x5a000;
        p[2] = 0xfeff2000;
        func_020731dc(p, func_020072c0, data_ov009_02113d44);
    }
    {
        *(unsigned int*)((char*)data_ov009_02113e34 + 0) = 0xfead4000;
        *(unsigned int*)((char*)data_ov009_02113e34 + 4) = (unsigned int)-0x320000;
        *(unsigned int*)((char*)data_ov009_02113e34 + 8) = 0xff682000;
        func_020731dc(data_ov009_02113e34, func_020072c0, data_ov009_02113d2c);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e40;
        p[0] = 0xfebb0000;
        p[1] = (unsigned int)-0x320000;
        p[2] = 0xff5ba000;
        func_020731dc(p, func_020072c0, data_ov009_02113d20);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e4c;
        p[0] = 0xfec5a000;
        p[1] = (unsigned int)-0x320000;
        p[2] = 0xff4d4000;
        func_020731dc(p, func_020072c0, data_ov009_02113c90);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e58;
        p[0] = 0xfecdc000;
        p[1] = (unsigned int)-0x320000;
        p[2] = (unsigned int)-0xc30000;
        func_020731dc(p, func_020072c0, data_ov009_02113c84);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e64;
        p[0] = 0xfed54000;
        p[1] = (unsigned int)-0x320000;
        p[2] = 0xff2cc000;
        func_020731dc(p, func_020072c0, data_ov009_02113cfc);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e70;
        p[0] = 0xfed54000;
        p[1] = (unsigned int)-0x320000;
        p[2] = 0xff25e000;
        func_020731dc(p, func_020072c0, data_ov009_02113cf0);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e7c;
        p[0] = 0xfed68000;
        p[1] = (unsigned int)-0x320000;
        p[2] = 0xff1aa000;
        func_020731dc(p, func_020072c0, data_ov009_02113cb4);
    }
    {
        *(unsigned int*)((char*)data_ov009_02113de0 + 0) = 0xfeb2e000;
        *(unsigned int*)((char*)data_ov009_02113de0 + 4) = 0xffbd2000;
        *(unsigned int*)((char*)data_ov009_02113de0 + 8) = (unsigned int)-0x960000;
        func_020731dc(data_ov009_02113de0, func_020072c0, data_ov009_02113ccc);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113dec;
        p[0] = 0xfebf6000;
        p[1] = 0xffbd2000;
        p[2] = 0xff5ce000;
        func_020731dc(p, func_020072c0, data_ov009_02113cd8);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113df8;
        p[0] = 0xfec96000;
        p[1] = 0xffbd2000;
        p[2] = 0xff4e8000;
        func_020731dc(p, func_020072c0, data_ov009_02113ce4);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e04;
        p[0] = 0xfed0e000;
        p[1] = 0xffbd2000;
        p[2] = 0xff3ee000;
        func_020731dc(p, func_020072c0, data_ov009_02113d68);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e10;
        p[0] = 0xfed7c000;
        p[1] = 0xffbd2000;
        p[2] = 0xff2f4000;
        func_020731dc(p, func_020072c0, data_ov009_02113ca8);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e1c;
        p[0] = 0xfed9a000;
        p[1] = 0xffbd2000;
        p[2] = 0xff1e6000;
        func_020731dc(p, func_020072c0, data_ov009_02113d38);
    }
    {
        unsigned int *p = (unsigned int*)data_ov009_02113e28;
        p[0] = 0xfed72000;
        p[1] = 0xffbd2000;
        p[2] = 0xff0ce000;
        func_020731dc(p, func_020072c0, data_ov009_02113c9c);
    }
}
