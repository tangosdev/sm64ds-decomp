struct V2 { int x, y; };
extern int func_02018e3c(void *out_file_id, const char *path);
extern int func_020182ec(struct V2 file, int flag, ...);

void *func_020182bc(const char *path)
{
    struct V2 file;
    func_02018e3c(&file, path);
    return (void *)func_020182ec(file, 1);
}
