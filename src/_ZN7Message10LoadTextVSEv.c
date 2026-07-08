extern int GetOwnerLanguage(void);
extern void _ZN7Message16SetTextGlobalsVSEv(void);
extern int data_020937bc;
extern int data_0209325c;
extern int data_02093d7c;
extern int data_0209289c;
extern int data_02092d3c;
extern int* VS_MSG_FILE_PTR;

void _ZN7Message10LoadTextVSEv(void) {
    if (GetOwnerLanguage() == 5) {
        VS_MSG_FILE_PTR = &data_020937bc;
    } else if (GetOwnerLanguage() == 4) {
        VS_MSG_FILE_PTR = &data_0209325c;
    } else if (GetOwnerLanguage() == 3) {
        VS_MSG_FILE_PTR = &data_02093d7c;
    } else if (GetOwnerLanguage() == 2) {
        VS_MSG_FILE_PTR = &data_0209289c;
    } else {
        VS_MSG_FILE_PTR = &data_02092d3c;
    }
    _ZN7Message16SetTextGlobalsVSEv();
}
