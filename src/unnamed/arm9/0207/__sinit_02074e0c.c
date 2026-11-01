extern void *func_0201aa18();
extern void __register_global_object();
extern int data_0209d574[];
extern int func_0201aaec[];
extern int data_0209d528[];
/* Static initializer for the watchdog alarm data_0209d574: construct it
   (func_0201aa18) and register its destructor func_0201aaec. Its owner is
   the unpromoted shard run between IRQ2 and Message2, so it cannot fold yet
   (notes/mwccarm-codegen.md 6dg, "Not done"). */
void __sinit_02074e0c(void)
{
    func_0201aa18(data_0209d574);
    __register_global_object(data_0209d574, func_0201aaec, data_0209d528);
}
