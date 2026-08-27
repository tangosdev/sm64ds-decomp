struct S { int a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22; };
extern struct S data_ov006_02133810;
extern struct S data_ov006_021425a8;
void __sinit_ov006_0213195c(void);
void __sinit_ov006_0213195c(void) {
    struct S *s = &data_ov006_02133810;
    struct S *d = &data_ov006_021425a8;
    d->a0=s->a0; d->a1=s->a1; d->a2=s->a2;
    d->a3=s->a11; d->a4=s->a12; d->a5=s->a13; d->a6=s->a14;
    d->a7=s->a3; d->a8=s->a4; d->a9=s->a5; d->a10=s->a6;
    d->a11=s->a9; d->a12=s->a10;
    d->a13=s->a7; d->a14=s->a8;
    d->a15=s->a15; d->a16=s->a16; d->a17=s->a17; d->a18=s->a18;
    d->a19=s->a19; d->a20=s->a20; d->a21=s->a21; d->a22=s->a22;
}
