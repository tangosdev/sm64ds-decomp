/* recovered: the path RIDES THROUGH r1. The body saves r0 (the FSFile) in r4, puts
 * its own stack FSFileID in r0 and calls func_0205d644 without touching r1, so the
 * path its caller left there is the one that gets looked up. The one-argument call
 * was byte-exact and lost the path on any stack-argument host. */
typedef struct { int a; int b; } FSFileID;

extern int func_0205d644(FSFileID *out, const char *path);
extern int func_0205d568(void *file, FSFileID id);

int func_0205d518(void *file, const char *path)
{
    FSFileID id;
    if (func_0205d644(&id, path) != 0) {
        if (func_0205d568(file, id) != 0)
            return 1;
    }
    return 0;
}
