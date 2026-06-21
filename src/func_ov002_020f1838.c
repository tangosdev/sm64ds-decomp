/* func_ov002_020f1838 at 0x020f1838
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct SharedFilePtr;
extern struct SharedFilePtr data_ov002_0210d9e0;
extern int _ZN5Event6GetBitEj(unsigned int n);
extern void _ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr *p);

int func_ov002_020f1838(char *r0) {
    char *r4 = r0;
    *(unsigned short*)(r4 + 0xd4) = 0xce;
    *(unsigned char*)(r4 + 0xdc) = (short)*(short*)(r4 + 0x90) & 0x1f;
    *(int*)(r4 + 0xd8) = _ZN5Event6GetBitEj(*(unsigned char*)(r4 + 0xdc));
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
    return 1;
}
