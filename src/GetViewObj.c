struct E { unsigned char d[0xe]; };
extern struct E *VIEW_ARR_PTR;
struct E *GetViewObj(int idx) {
    return &VIEW_ARR_PTR[idx];
}
