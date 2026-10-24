/* storage 0x023895DC runtime 0x03801474 size 0x1CC + 8 pool wram ARM.
 * Pools (0x038094E8 x2, 0xFFFF x3). In-pack? No: callees are all out of
 * pack (fn_023882B0/fn_023883FC/fn_02389D60/fn_02389D50). Data
 * unk_38094E8 = 0x038094E8. fn_023882B0 takes 4 or 5 args (5th on stack)
 * so it is declared variadic. Case blocks in source order 60,61,64,65,
 * 63,66,67,default to match the ROM layout (like fn_02389454's a,d,e,c).
 */

extern int fn_023882B0(int a1, int a2, int a3, int a4, ...);
extern int fn_023883FC(int a1, int a2);
extern int fn_02389D60(int a1);
extern int fn_02389D50(void);
extern unsigned short unk_38094E8[];

void fn_023895DC(int a1) {
    if ((a1 & 0x2000000) != 0) {
        int i = 0;
        do {
            unk_38094E8[i] = 0;
            i++;
        } while (i < 16);
    }
    unk_38094E8[((unsigned)(a1 & 0xF0000)) >> 16] = (unsigned short)a1;
    if ((a1 & 0x1000000) == 0)
        return;
    {
        int v4 = unk_38094E8[0];
        unsigned int v5 = ((unsigned)(v4 & 0xFF00) << 8) >> 16;
        switch (v5) {
        case 0x60:
            fn_023883FC(0x60, 0);
            break;
        case 0x61:
            if (fn_023882B0(3, v5, 2, (unsigned char)v4, unk_38094E8[1] & 0xFFFF) == 0)
                fn_023883FC(v5, 4);
            break;
        case 0x64:
            if (fn_023882B0(3, v5, 2, (unsigned char)v4, unk_38094E8[1] & 0xFFFF) == 0)
                fn_023883FC(v5, 4);
            break;
        case 0x65:
            if (fn_023882B0(3, v5, 1, v4 & 0xFFFF) == 0)
                fn_023883FC(v5, 4);
            break;
        case 0x63: {
            int b = v4 & 0xFF;
            int h = unk_38094E8[1] & 0xFFFF;
            if (fn_023882B0(3, v5, 1, (b << 16) | h) == 0)
                fn_023883FC(v5, 4);
            break;
        }
        case 0x66:
            fn_02389D60((unsigned char)v4);
            fn_023883FC(0x66, 0);
            break;
        case 0x67:
            fn_023883FC(0x67, (unsigned short)fn_02389D50());
            break;
        default:
            fn_023883FC(v5, 1);
            break;
        }
    }
}
