extern int WM_StartConnect(void *callback, int pInfo, void *ssid);
struct WmStartConnectCallback;
extern void func_0203fec4(struct WmStartConnectCallback *cb);
extern int data_020a0f54;

/* Connect to the chosen parent: WM_StartConnect on the
   WMBssDesc in data_020a0f54, no SSID, with func_0203fec4 as the callback.
   1 when the request is accepted (2). */
int func_0203fb5c(void) {
    return WM_StartConnect((void *)func_0203fec4, data_020a0f54, 0) == 2;
}
