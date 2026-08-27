extern int _ZN5Sound12PlayBank2_2DEj(unsigned);
extern unsigned char data_ov006_0212efd4[];

typedef struct Elem {
    int word0;
    short pad04;
    unsigned short cnt;
    unsigned short h08;
    short pad0a;
    unsigned char b0c;
    unsigned char pad0d;
    unsigned char b0e;
    unsigned char pad0f;
    unsigned char pad10;
    unsigned char b11;
    unsigned char pad12[0xe];
} Elem;

typedef struct S {
    char pad[0x4bac];
    Elem arr[2];
} S;

void func_ov006_0211dad0(S *s, int i) {
    s->arr[i].cnt++;
    if (s->arr[i].cnt < 8) return;
    s->arr[i].cnt = 0;
    s->arr[i].b11++;
    if (s->arr[i].b11 >= 8) {
        s->arr[i].b11 = 0;
        s->arr[i].b0c = 0;
        s->arr[i].b0e = 3;
        s->arr[i].h08 = 0;
        s->arr[i].word0 = 0x1000;
        _ZN5Sound12PlayBank2_2DEj(0x1f9);
    } else {
        s->arr[i].b0c = data_ov006_0212efd4[s->arr[i].b11];
    }
}
