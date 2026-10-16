/* fn_02388384: acknowledge when the pending request matches. */

extern int dword_3808FC4[];
extern int word_380941C[];

extern int fn_02380FE0(int *);

int fn_02388384(int result)
{
    if (dword_3808FC4[1] == result) {
        dword_3808FC4[1] = 5;
        dword_3808FC4[0] = 0;
        return fn_02380FE0(word_380941C);
    }
    return result;
}
