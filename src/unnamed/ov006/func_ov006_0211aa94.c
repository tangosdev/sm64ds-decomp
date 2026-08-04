typedef struct Entry {
    int b4;
    int unk4;
    int bc;
    int c0;
    int unk10;
    int unk14;
    unsigned char cc;
    unsigned char cd;
    unsigned char pad[10];
} Entry;

typedef struct Big {
    char pad[0x51b4];
    Entry arr[16];
} Big;

void func_ov006_0211aa94(Big *this, int idx) {
    this->arr[idx].b4 += this->arr[idx].bc;
    this->arr[idx].bc += 0x100;
    if (((this->arr[idx].c0 - this->arr[idx].b4) >> 12) <= 0x18) {
        this->arr[idx].cc = 0;
        this->arr[idx].cd = 0;
    }
}
