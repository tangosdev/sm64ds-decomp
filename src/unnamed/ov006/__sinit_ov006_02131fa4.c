struct Pair { int a, b; };
struct Trip { int a, b, c; };

extern int data_ov006_02136cf0[3];
extern int data_ov006_02136d1c[3];

extern struct Pair data_ov006_0213dd0c;
extern struct Pair data_ov006_0213dc4c;
extern struct Pair data_ov006_0213dc5c;
extern struct Pair data_ov006_0213dc54;
extern struct Pair data_ov006_0213dc74;
extern struct Pair data_ov006_0213dc6c;
extern struct Pair data_ov006_0213dc84;
extern struct Pair data_ov006_0213dccc;

extern struct Pair data_ov006_0213dd14;
extern struct Pair data_ov006_0213dce4;
extern struct Pair data_ov006_0213dcfc;
extern struct Pair data_ov006_0213dd1c;

extern struct Pair data_ov006_0213dcac;
extern struct Pair data_ov006_0213dcdc;
extern struct Pair data_ov006_0213dcec;
extern struct Pair data_ov006_0213dc9c;

extern struct Pair data_ov006_0213dcd4;
extern struct Pair data_ov006_0213dcc4;
extern struct Pair data_ov006_0213dc7c;
extern struct Pair data_ov006_0213dca4;
extern struct Pair data_ov006_0213dcb4;

extern struct Pair data_ov006_0213dc8c;
extern struct Pair data_ov006_0213dd04;
extern struct Pair data_ov006_0213dd2c;

extern struct Pair data_ov006_0213dcbc;
extern struct Pair data_ov006_0213dcf4;
extern struct Pair data_ov006_0213dd24;

struct DstA { int x[3]; int y[3]; };
struct Dst8 { struct Pair p0, p1, p2, p3, p4, p5, p6, p7; };
struct Dst4 { struct Pair p0, p1, p2, p3; };
struct Dst5 { struct Pair p0, p1, p2, p3, p4; };
struct Dst3 { struct Pair p0, p1, p2; };

extern struct DstA data_ov006_021427d4;
extern struct Dst8 data_ov006_02142888;
extern struct Dst4 data_ov006_02142840;
extern struct Dst4 data_ov006_02142820;
extern struct Dst5 data_ov006_02142860;
extern struct Dst3 data_ov006_021427ec;
extern struct Dst3 data_ov006_021427bc;

void __sinit_ov006_02131fa4(void)
{
    data_ov006_021427d4.x[0] = data_ov006_02136cf0[0];
    data_ov006_021427d4.x[1] = data_ov006_02136cf0[1];
    data_ov006_021427d4.x[2] = data_ov006_02136cf0[2];
    data_ov006_021427d4.y[0] = data_ov006_02136d1c[0];
    data_ov006_021427d4.y[1] = data_ov006_02136d1c[1];
    data_ov006_021427d4.y[2] = data_ov006_02136d1c[2];

    data_ov006_02142888.p0 = data_ov006_0213dd0c;
    data_ov006_02142888.p1 = data_ov006_0213dc4c;
    data_ov006_02142888.p2 = data_ov006_0213dc5c;
    data_ov006_02142888.p3 = data_ov006_0213dc54;
    data_ov006_02142888.p4 = data_ov006_0213dc74;
    data_ov006_02142888.p5 = data_ov006_0213dc6c;
    data_ov006_02142888.p6 = data_ov006_0213dc84;
    data_ov006_02142888.p7 = data_ov006_0213dccc;

    data_ov006_02142840.p0 = data_ov006_0213dd14;
    data_ov006_02142840.p1 = data_ov006_0213dce4;
    data_ov006_02142840.p2 = data_ov006_0213dcfc;
    data_ov006_02142840.p3 = data_ov006_0213dd1c;

    data_ov006_02142820.p0 = data_ov006_0213dcac;
    data_ov006_02142820.p1 = data_ov006_0213dcdc;
    data_ov006_02142820.p2 = data_ov006_0213dcec;
    data_ov006_02142820.p3 = data_ov006_0213dc9c;

    data_ov006_02142860.p0 = data_ov006_0213dcd4;
    data_ov006_02142860.p1 = data_ov006_0213dcc4;
    data_ov006_02142860.p2 = data_ov006_0213dc7c;
    data_ov006_02142860.p3 = data_ov006_0213dca4;
    data_ov006_02142860.p4 = data_ov006_0213dcb4;

    data_ov006_021427ec.p0 = data_ov006_0213dc8c;
    data_ov006_021427ec.p1 = data_ov006_0213dd04;
    data_ov006_021427ec.p2 = data_ov006_0213dd2c;

    data_ov006_021427bc.p0 = data_ov006_0213dcbc;
    data_ov006_021427bc.p1 = data_ov006_0213dcf4;
    data_ov006_021427bc.p2 = data_ov006_0213dd24;
}
