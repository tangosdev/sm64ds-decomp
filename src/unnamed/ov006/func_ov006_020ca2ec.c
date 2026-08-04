typedef struct { int a, b; } S8;
extern S8 data_ov006_0213b134;
extern void func_ov006_020ca070(void *this);

void func_ov006_020ca2ec(void *this) {
    *(S8 *)((char *)this + 0x70) = data_ov006_0213b134;
    func_ov006_020ca070(this);
}
