//cpp
// func_ov060_02115a30 at 0x02115a30 -- matched byte-for-byte with mwccarm 1.2/sp2p3 (ov060).
class Animation {
public:
    int Finished();
    int GetFrameCount() const;
    int WillHitFrame(int) const;
};

struct Obj {
    char pad[0x124];
    Animation anim;
};

extern "C" bool func_ov060_02115a30(Obj* o) {
    return o->anim.Finished() || o->anim.WillHitFrame((unsigned short)(o->anim.GetFrameCount() - 1));
}
