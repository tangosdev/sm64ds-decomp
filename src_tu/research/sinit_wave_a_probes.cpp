// Compiler-only ownership probes for three reconstructed static initializers.
//
// This file is intentionally absent from every TU manifest and delinks list.
// Compile exactly one SINIT_PROBE_* define at a time.  The declarations below
// are ordinary C++ global initializers: no __sinit body is written by hand.

#if defined(SINIT_PROBE_ONEUP)

struct OneUpProbe {
    void state00();
    void state01();
    void state02();
    void state03();
    void state04();
    void state05();
    void state06();
    void state07();
    void state08();
    void state09();
    void state10();
    void state11();
    void state12();
    void state13();
};

typedef void (OneUpProbe::*OneUpState)();

OneUpState oneUpStates[14] = {
    &OneUpProbe::state00,
    &OneUpProbe::state01,
    &OneUpProbe::state02,
    &OneUpProbe::state03,
    &OneUpProbe::state04,
    &OneUpProbe::state05,
    &OneUpProbe::state06,
    &OneUpProbe::state07,
    &OneUpProbe::state08,
    &OneUpProbe::state09,
    &OneUpProbe::state10,
    &OneUpProbe::state11,
    &OneUpProbe::state12,
    &OneUpProbe::state13,
};

#elif defined(SINIT_PROBE_KURUMAJIKU)

// Four separate objects are required.  An array would register one array
// destruction thunk instead of four Vector3 destructors.
// The ROM performs the component stores at startup, so an aggregate
// initializer (which places the components directly in .data) is insufficient.
// An inline three-component constructor generates those stores organically.
struct KurumajikuVectorProbe {
    int x, y, z;

    KurumajikuVectorProbe(int x_, int y_, int z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }
    ~KurumajikuVectorProbe() {}
};

KurumajikuVectorProbe kurumajikuDirection0( 0,        0x190000, 0x12c000);
KurumajikuVectorProbe kurumajikuDirection1( 0,       -0x190000, 0x12c000);
KurumajikuVectorProbe kurumajikuDirection2( 0x190000, 0,        0x12c000);
KurumajikuVectorProbe kurumajikuDirection3(-0x190000, 0,        0x12c000);

#elif defined(SINIT_PROBE_MADPIANO)

// The ROM calls three distinct constructor/destructor pairs.  Separate probe
// types preserve that fact while leaving the still-unknown production class
// names and signatures unclaimed.
struct ModelFileProbe {
    unsigned words[2];
    ModelFileProbe(unsigned assetId);
    ~ModelFileProbe();
};

struct CollisionFileProbe {
    unsigned words[2];
    CollisionFileProbe(unsigned assetId);
    ~CollisionFileProbe();
};

struct AnimationFileProbe {
    unsigned words[2];
    AnimationFileProbe(unsigned assetId);
    ~AnimationFileProbe();
};

ModelFileProbe pianoModelFile(0x40a);
CollisionFileProbe pianoCollisionFile(0x40b);
AnimationFileProbe pianoAttackAnimationFile(0x40c);

struct MadPianoProbe {
    void state0();
    void state1();
    void state2();
    void state3();
};

typedef void (MadPianoProbe::*MadPianoState)();
struct MadPianoStateEntry {
    MadPianoState states[2];
};

MadPianoStateEntry madPianoStates[2] = {
    { &MadPianoProbe::state0, &MadPianoProbe::state1 },
    { &MadPianoProbe::state2, &MadPianoProbe::state3 },
};

#endif
