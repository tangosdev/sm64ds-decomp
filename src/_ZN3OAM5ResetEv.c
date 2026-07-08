//cpp
// NONMATCHING: different op / idiom (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
void MultiCopy_Int(int *dst, int *src, int len);
void MultiCopy32Bytes(int *src, int *dst, int len);
}

struct OAMEntry { u32 a; u16 b; u16 pad; };

extern u8 _ZN3OAM14subOAMDisabledE;
extern OAMEntry _ZN3OAM10bufferMainE[];
extern int data_0209e67c[];
extern int data_0209e694[];
extern int data_0209ea74[];
extern int _ZN3OAM11objCountSubE;
extern int _ZN3OAM15curAffineSetSubE;
extern int _ZN3OAM12objCountMainE;
extern int _ZN3OAM16curAffineSetMainE;

namespace OAM {
void Reset() {
    int i;
    if (_ZN3OAM14subOAMDisabledE != 0) {
        for (i = 0; i < 0x80; i++) {
            _ZN3OAM10bufferMainE[i].a = 0xc0;
            _ZN3OAM10bufferMainE[i].b = 0;
        }
    } else {
        _ZN3OAM10bufferMainE[0].a = 0xc0;
        _ZN3OAM10bufferMainE[0].b = 0;
        MultiCopy_Int((int*)_ZN3OAM10bufferMainE, data_0209e67c, 0x18);
    }
    MultiCopy32Bytes((int*)_ZN3OAM10bufferMainE, data_0209e694, 0x3e0);
    MultiCopy32Bytes((int*)_ZN3OAM10bufferMainE, data_0209ea74, 0x400);
    _ZN3OAM11objCountSubE = _ZN3OAM15curAffineSetSubE = 0;
    _ZN3OAM12objCountMainE = _ZN3OAM16curAffineSetMainE = 0;
}
}
