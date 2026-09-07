//cpp
/* Notes-only organic-generation probe. It contributes nothing unless the
   explicit research define below is supplied to a manual compiler command. */
#ifdef SINIT_OWNERSHIP_PROBE
class MarioCapAnimFileProbe
{
    int words[2];

public:
    MarioCapAnimFileProbe(unsigned int fileID);
    ~MarioCapAnimFileProbe();
};

struct MarioCapStateProbe
{
    int state00();
    int state01();
    int state02();
    int state03();
    int state04();
    int state05();
    int state06();
    int state07();
    int state08();
    int state09();
    int state10();
    int state11();
    int state12();
    int state13();
    int state14();
    int state15();
    int state16();
    int state17();
};

typedef int (MarioCapStateProbe::*MarioCapStatePMF)();

MarioCapAnimFileProbe data_ov002_0210de30__probe(0x8012);
MarioCapAnimFileProbe data_ov002_0210de38__probe(0x8013);
MarioCapAnimFileProbe data_ov002_0210de50__probe(0x480);
MarioCapAnimFileProbe data_ov002_0210de60__probe(0x47e);
MarioCapAnimFileProbe data_ov002_0210de48__probe(0x47f);
MarioCapAnimFileProbe data_ov002_0210de28__probe(0x478);
MarioCapAnimFileProbe data_ov002_0210de08__probe(0x476);
MarioCapAnimFileProbe data_ov002_0210de20__probe(0x47c);
MarioCapAnimFileProbe data_ov002_0210de40__probe(0x479);
MarioCapAnimFileProbe data_ov002_0210de10__probe(0x477);
MarioCapAnimFileProbe data_ov002_0210de00__probe(0x47d);
MarioCapAnimFileProbe data_ov002_0210de58__probe(0x47b);
MarioCapAnimFileProbe data_ov002_0210de18__probe(0x47a);

MarioCapStatePMF data_ov002_0210df64__probe[2] = {
    &MarioCapStateProbe::state00, &MarioCapStateProbe::state01,
};
MarioCapStatePMF data_ov002_0210df84__probe[2] = {
    &MarioCapStateProbe::state02, &MarioCapStateProbe::state03,
};
MarioCapStatePMF data_ov002_0210df04__probe[2] = {
    &MarioCapStateProbe::state04, &MarioCapStateProbe::state05,
};
MarioCapStatePMF data_ov002_0210df24__probe[2] = {
    &MarioCapStateProbe::state06, &MarioCapStateProbe::state07,
};
MarioCapStatePMF data_ov002_0210df34__probe[2] = {
    &MarioCapStateProbe::state08, &MarioCapStateProbe::state09,
};
MarioCapStatePMF data_ov002_0210df54__probe[2] = {
    &MarioCapStateProbe::state10, &MarioCapStateProbe::state11,
};
MarioCapStatePMF data_ov002_0210df74__probe[2] = {
    &MarioCapStateProbe::state12, &MarioCapStateProbe::state13,
};
MarioCapStatePMF data_ov002_0210df14__probe[2] = {
    &MarioCapStateProbe::state14, &MarioCapStateProbe::state15,
};
MarioCapStatePMF data_ov002_0210df44__probe[2] = {
    &MarioCapStateProbe::state16, &MarioCapStateProbe::state17,
};
#endif
