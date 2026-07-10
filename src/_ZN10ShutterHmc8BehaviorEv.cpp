//cpp
extern "C" {
int func_ov002_020bac18(void);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN10ShutterHmc8BehaviorEv(void* c){
  int r = func_ov002_020bac18();
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  return r;
}
}
