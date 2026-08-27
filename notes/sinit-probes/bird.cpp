//cpp
/* Notes-only organic-generation probe. It contributes nothing unless the
   explicit research define below is supplied to a manual compiler command. */
#ifdef SINIT_OWNERSHIP_PROBE
class BirdModelFileProbe
{
    int words[2];

public:
    BirdModelFileProbe(unsigned int fileID);
    ~BirdModelFileProbe();
};

class BirdAnimFileProbe
{
    int words[2];

public:
    BirdAnimFileProbe(unsigned int fileID);
    ~BirdAnimFileProbe();
};

struct BirdStateProbe
{
    void state0();
    void state1();
    void state2();
    void state3();
};

typedef void (BirdStateProbe::*BirdStatePMF)();

BirdModelFileProbe data_ov009_02113c20__probe(1080);
BirdAnimFileProbe data_ov009_02113c28__probe(1081);
BirdStatePMF data_ov009_02113c48__probe[4] = {
    &BirdStateProbe::state0,
    &BirdStateProbe::state1,
    &BirdStateProbe::state2,
    &BirdStateProbe::state3,
};
#endif
