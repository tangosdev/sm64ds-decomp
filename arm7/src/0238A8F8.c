/* storage 0x0238A8F8 runtime 0x03802790 size 0x128 wram ARM.
 * Channel control word. Bits 8-13 select the case and bit 14 is the flag.
 * Reading the selector as a bitfield keeps the loaded word in r1. The
 * default arm reloads through volatile so the first clear is not forwarded. */

struct ChWord {
    unsigned lo : 8;
    unsigned mode : 6;
    unsigned en : 1;
    unsigned hi : 17;
};

int *fn_0238A8F8(int *p) {
    struct ChWord *c = (struct ChWord *)p;
    unsigned mode = c->mode;
    unsigned w;

    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 16:
    case 17:
        w = *(unsigned int *)c;
        w &= ~0x4000u;
        *(unsigned int *)c = w;
        break;
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 34:
    case 35:
        w = *(unsigned int *)c;
        w |= 0x4000u;
        *(unsigned int *)c = w;
        w = *(unsigned int *)c;
        *(unsigned int *)c = (w & ~0x3F00u) | ((((w << 18) >> 26) - 0x12) & 0x3F) << 8;
        break;
    case 32:
    case 33:
        w = *(unsigned int *)c;
        w |= 0x4000u;
        *(unsigned int *)c = w;
        w = *(unsigned int *)c;
        *(unsigned int *)c = (w & ~0x3F00u) | ((((w << 18) >> 26) - 0x18) & 0x3F) << 8;
        break;
    default:
        w = *(volatile unsigned int *)p;
        w &= ~0x4000u;
        *p = w;
        w = *(volatile unsigned int *)p;
        w &= ~0x3F00u;
        *p = w;
        break;
    }
    return p;
}
