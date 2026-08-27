struct Pair { int a, b; };
extern struct Pair data_ov006_0213ca64;
void func_ov006_020ed32c(char *p) {
    *(struct Pair *)(p + 0x4000 + 0x660) = data_ov006_0213ca64;
}
