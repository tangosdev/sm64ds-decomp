//cpp
// Bowser_IsAnimAtLastFrame at 0x02115a30 -- matched byte-for-byte with mwccarm 1.2/sp2p3 (ov060).
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

/* int, not bool: all 17 cross-TU declarations of this read it wide, and the
   ROM writes the whole of r0 on both return paths (+0x3c mov r0,#1,
   +0x48 mov r0,#0). A C++ bool here would return the answer in AL alone. */
extern "C" int Bowser_IsAnimAtLastFrame(Obj* o) {
    return o->anim.Finished() || o->anim.WillHitFrame((unsigned short)(o->anim.GetFrameCount() - 1));
}
