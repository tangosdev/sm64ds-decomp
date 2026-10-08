/* .data descriptor records and the runtime state tables they populate are
   both 8-byte pointer-to-member records: Pmf is the raw pair, Pmf0 the same
   bit pattern for a no-argument handler, PmfEntry wraps it. */
typedef struct { int a, b; } Pmf;
typedef Pmf Pmf0;
typedef struct { Pmf0 pmf; } PmfEntry;

extern Pmf data_ov006_0213f904;
extern Pmf data_ov006_0213f92c;
extern Pmf data_ov006_0213f954;
extern Pmf data_ov006_0213f964;
extern Pmf data_ov006_0213f914;

extern Pmf data_ov006_0213f934;
extern Pmf data_ov006_0213f8dc;
extern Pmf data_ov006_0213f94c;
extern Pmf data_ov006_0213f8ec;
extern Pmf data_ov006_0213f8e4;
extern Pmf data_ov006_0213f8d4;
extern Pmf data_ov006_0213f93c;
extern Pmf data_ov006_0213f96c;

extern Pmf data_ov006_0213f99c;
extern Pmf data_ov006_0213f994;
extern Pmf data_ov006_0213f98c;
extern Pmf data_ov006_0213f984;
extern Pmf data_ov006_0213f97c;

extern Pmf data_ov006_0213f90c;
extern Pmf data_ov006_0213f95c;
extern Pmf data_ov006_0213f91c;
extern Pmf data_ov006_0213f8fc;
extern Pmf data_ov006_0213f944;
extern Pmf data_ov006_0213f8f4;
extern Pmf data_ov006_0213f9ac;
extern Pmf data_ov006_0213f9a4;
extern Pmf data_ov006_0213f924;

extern PmfEntry data_ov006_02142eb0[5];
extern Pmf data_ov006_02142e88[5];
extern Pmf data_ov006_02142ed8[8];
extern Pmf data_ov006_02142f18[9];

void __sinit_ov006_02132f68(void)
{
    data_ov006_02142eb0[0].pmf = data_ov006_0213f904;
    data_ov006_02142eb0[1].pmf = data_ov006_0213f92c;
    data_ov006_02142eb0[2].pmf = data_ov006_0213f954;
    data_ov006_02142eb0[3].pmf = data_ov006_0213f964;
    data_ov006_02142eb0[4].pmf = data_ov006_0213f914;

    data_ov006_02142ed8[0] = data_ov006_0213f934;
    data_ov006_02142ed8[1] = data_ov006_0213f8dc;
    data_ov006_02142ed8[2] = data_ov006_0213f94c;
    data_ov006_02142ed8[3] = data_ov006_0213f8ec;
    data_ov006_02142ed8[4] = data_ov006_0213f8e4;
    data_ov006_02142ed8[5] = data_ov006_0213f8d4;
    data_ov006_02142ed8[6] = data_ov006_0213f93c;
    data_ov006_02142ed8[7] = data_ov006_0213f96c;

    data_ov006_02142e88[0] = data_ov006_0213f99c;
    data_ov006_02142e88[1] = data_ov006_0213f994;
    data_ov006_02142e88[2] = data_ov006_0213f98c;
    data_ov006_02142e88[3] = data_ov006_0213f984;
    data_ov006_02142e88[4] = data_ov006_0213f97c;

    data_ov006_02142f18[0] = data_ov006_0213f90c;
    data_ov006_02142f18[1] = data_ov006_0213f95c;
    data_ov006_02142f18[2] = data_ov006_0213f91c;
    data_ov006_02142f18[3] = data_ov006_0213f8fc;
    data_ov006_02142f18[4] = data_ov006_0213f944;
    data_ov006_02142f18[5] = data_ov006_0213f8f4;
    data_ov006_02142f18[6] = data_ov006_0213f9ac;
    data_ov006_02142f18[7] = data_ov006_0213f9a4;
    data_ov006_02142f18[8] = data_ov006_0213f924;
}
