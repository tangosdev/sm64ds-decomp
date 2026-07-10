// Serializes a small record into a byte stream: writes the type byte, then
// for type 4 appends the two halfwords at +2 and +4 little-endian. Returns
// the advanced stream pointer, or 0 for type 0 / unknown types.
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct Rec {
    u8 type;
    char _pad;
    u16 a;
    u16 b;
} Rec;

u8 *func_02065d5c(Rec *src, u8 *dst)
{
    u8 *d = dst;
    *d++ = src->type;
    switch (src->type) {
    case 4:
        *d++ = src->a;
        *d++ = (src->a & 0xff00) >> 8;
        *d++ = src->b;
        *d++ = (src->b & 0xff00) >> 8;
        break;
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
        break;
    default:
        return 0;
    }
    return d;
}
