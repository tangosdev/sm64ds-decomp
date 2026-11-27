extern int fn_02381600(int, int, int);
extern int unk_38070C8;
extern char aFailed_3805E60[];
extern int (*off_3805E58)(char *);

void fn_02383B08(void) {
    if (fn_02381600((int)&unk_38070C8, 1, 0) == 0)
        off_3805E58(aFailed_3805E60);
}
