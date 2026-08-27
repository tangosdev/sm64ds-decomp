struct E { unsigned char d[0xe]; };
extern struct E *data_0209f354;
struct E *GetViewObj(int idx) {
    return &data_0209f354[idx];
}
