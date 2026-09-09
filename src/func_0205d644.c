/* recovered: r0 is the FSFileID out pointer, r1 the path; func_0205d714 is
 * handed them as (file, path, out_file_id, 0). Its one-instruction caller
 * func_02018e3c reaches it with both still in the registers. */
extern void FS_InitFile(void *file);
extern int func_0205d714(void *file, const char *path, void *out_file_id, int c);

int func_0205d644(void *out_file_id, const char *path){
  int file[0x11];
  FS_InitFile(file);
  return func_0205d714(file, path, out_file_id, 0) != 0;
}
