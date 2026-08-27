extern int data_0209b284[];
extern unsigned char data_0209f21c[];
extern unsigned char data_0209b270[];
extern int data_0209b2a4[];
extern int data_0209fc4c[];
extern int data_0209fc48[];

void ResetKuppaScript(void)
{
    int i;
    int j;

    for (i = 0; i < 4; i++) data_0209b284[i] = 0;
    data_0209b270[0] = data_0209f21c[0];
    for (j = 0; j < 0x10; j++) data_0209b2a4[j] = 0;
    data_0209fc48[0] = data_0209fc4c[0];
}
