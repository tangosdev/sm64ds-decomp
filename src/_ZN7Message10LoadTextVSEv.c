extern int GetOwnerLanguage(void);
extern void _ZN7Message16SetTextGlobalsVSEv(void);
extern int data_020937bc;
extern int data_0209325c;
extern int data_02093d7c;
extern int data_0209289c;
extern int data_02092d3c;
extern int* data_0209fd14;

void _ZN7Message10LoadTextVSEv(void) {
    if (GetOwnerLanguage() == 5) {
        data_0209fd14 = &data_020937bc;
    } else if (GetOwnerLanguage() == 4) {
        data_0209fd14 = &data_0209325c;
    } else if (GetOwnerLanguage() == 3) {
        data_0209fd14 = &data_02093d7c;
    } else if (GetOwnerLanguage() == 2) {
        data_0209fd14 = &data_0209289c;
    } else {
        data_0209fd14 = &data_02092d3c;
    }
    _ZN7Message16SetTextGlobalsVSEv();
}
