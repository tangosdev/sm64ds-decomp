//cpp
// _ZN13WaterfallMist6RenderEv at 0x020b83c4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" int _ZN5Model6RenderEPK7Vector3(void *, const void *);
extern "C" int data_ov002_0210df54;

extern "C" int _ZN13WaterfallMist6RenderEv(char *self)
{
    int b = (*(int *)(self + 0xb0) & 0x40000) ? 1 : 0;
    if (b) return 1;
    if (*(unsigned char *)(self + 0x3ff) == 1 || *(int *)(self + 0x80) < 0x100) return 1;

    int t = *(int *)(self + 0x3f0);
    if (t == 4 || t == 17 || t == 5 || t == 18 || t == 16 || t == 11 || (unsigned)(t - 6) <= 3) {
        if (!(*(int *)(self + 0x3ec) & 1) || *(int *)(self + 0x3bc) == (int)&data_ov002_0210df54) {
            _ZN5Model6RenderEPK7Vector3(self + 0x300, self + 0x80);
        }
    } else {
        _ZN5Model6RenderEPK7Vector3(self + 0x300, self + 0x80);
    }
    return 1;
}
