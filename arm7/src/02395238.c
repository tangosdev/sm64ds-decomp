#pragma scheduling off

extern unsigned short tbl_27D6554[];

unsigned short fn_02395238(int a, int b) {
    unsigned short t0;
    unsigned short mixed;
    unsigned short part;

    t0 = tbl_27D6554[b & 0xF];
    part = (unsigned short)(0xFFF & (b >> 4));
    mixed = (unsigned short)(t0 ^ part ^ tbl_27D6554[a & 0xF]);
    return (unsigned short)(tbl_27D6554[mixed & 0xF] ^
                            (unsigned short)(0xFFF & ((int)mixed >> 4)) ^
                            tbl_27D6554[(a >> 4) & 0xF]);
}
