"""Exercise the actual hosted fader class and its actual raw C dispatch on x86."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import msvc_env

ROOT = Path(__file__).resolve().parents[2]
PREFIX = r'''
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "FaderBrightness.h"
int g_hal_fader_stepping=1, steps=0, blends=0, notes=0;
const void *received=nullptr;
void hal_wipe_note(const char *,const void *self) {received=self;++notes;}
void Fader::AdvanceInterp() {received=this;++steps;currInterp+=speed;}
int FaderBrightness::IsAtStart() {return currInterp==0;}
int FaderBrightness::IsAtEnd() {return currInterp==0x1000;}
void FaderBrightness::SetToStart() {currInterp=0;}
void FaderBrightness::SetToEnd() {currInterp=0x1000;}
extern "C" void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p,
                                                 unsigned short value,short amount) {
    if((p!=(void*)0x04000050 && p!=(void*)0x04001050)||value!=0x3f||amount!=-1)
        std::abort();
    ++blends;
}
'''
SUFFIX = r'''
extern "C" {void *data_0209d4ac;void func_02018efc(void);}
static_assert(sizeof(void*)==4,"This is the production x86 calling convention");
static_assert(sizeof(HalFaderWipe)==0x60,"Seven hosted wipe objects retain their layout");
static int check(bool value,const char *what) {
    if(!value) {std::fprintf(stderr,"FAIL %s\n",what);return 1;}return 0;
}
__declspec(noinline) int other_slots(HalFaderWipe *p) {
    p->SetToStart();if(p->IsAtStart()!=1)return 1;
    p->SetForwardTime(32,0);if(p->speed!=128)return 1;
    p->SetToEnd();if(p->IsAtEnd()!=1)return 1;
    p->SetBackwardTime(16,0);if(p->speed!=-256)return 1;
    p->currInterp=0x800;if(p->IsBetweenStartAndEnd()!=1)return 1;
    p->DtorDeleting();p->HalTail28();p->HalTail2c();return 0;
}
void *foreign_seen=nullptr;
void __cdecl foreign_advance(void *self) {foreign_seen=self;}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    if(argv[1][0]=='i') {
        for(int i=0;i<7;++i)if(check(hal_wipe_index(&hal_wipes[i])==i,"array index"))return 1;
        HalFaderWipe foreign;
        std::uintptr_t first=reinterpret_cast<std::uintptr_t>(&hal_wipes[0]);
        const void *probes[]={nullptr,(void*)(first-1),(void*)(first+1),
                              (void*)(first+sizeof(hal_wipes)),(void*)&foreign};
        for(const void *p:probes)
            if(check(hal_wipe_index(p)==-1,"foreign/misaligned index"))return 1;
        puts("PASS index");return 0;
    }
    for(int i=0;i<7;++i) {
        HalFaderWipe *p=&hal_wipes[i];p->currInterp=0;p->speed=0x100;p->color=0;
        received=nullptr;steps=blends=0;data_0209d4ac=p;
        unsigned before,after;
        __asm {mov before,esp}
        __asm {mov ecx,012345678h}
        func_02018efc();
        __asm {mov after,esp}
        if(check(before==after && received==p && steps==1 && blends==2 && p->currInterp==0x100,
                 "raw C slot receiver/state/stack"))return 1;
        p->currInterp=0;received=nullptr;steps=blends=0;
        __asm {mov before,esp}
        int at_target=p->HalFaderWipe::AdvanceFade();
        __asm {mov after,esp}
        if(check(before==after && received==p && steps==1 && blends==2 && at_target==0,
                 "qualified slot receiver/stack"))return 1;
        __asm {mov before,esp}
        int failed=other_slots(p);
        __asm {mov after,esp}
        if(check(!failed && before==after,"remaining virtual slots"))return 1;
        g_hal_fader_stepping=0;p->speed=-1;notes=0;received=nullptr;func_02018efc();
        if(check(received==p && notes==1 && p->currInterp==0,"undriven slot"))return 1;
        g_hal_fader_stepping=1;
    }
    data_0209d4ac=nullptr;steps=0;func_02018efc();
    if(check(steps==0,"null installed object"))return 1;
    void *table[3]={nullptr,nullptr,(void*)&foreign_advance};
    struct {void **vtable;int marker;} foreign={table,123};
    data_0209d4ac=&foreign;func_02018efc();
    if(check(foreign_seen==&foreign,"other cdecl table"))return 1;
    puts("PASS actual_class_seven_objects");return 0;
}
'''


@unittest.skipUnless(os.name == "nt", "the host port uses Windows x86")
class FaderSlotABI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        source=(ROOT/"port/hal/fader_wipes.cpp").read_text(encoding="utf-8")
        start=source.index("struct HalFaderWipe {")
        end=source.index("\n};",start)+len("\n};")
        actual_class=source[start:end]
        start=source.index("int hal_wipe_index(const void *self)",end)
        end=source.index("\n}",start)+len("\n}")
        actual_index=source[start:end]
        (ROOT/"build").mkdir(exist_ok=True)
        cls.scratch=tempfile.TemporaryDirectory(prefix="fader-abi-test-",dir=ROOT/"build")
        cls.addClassCleanup(cls.scratch.cleanup)
        folder=Path(cls.scratch.name);cls.exes={}
        for variant in ("actual","wrong_abi"):
            body=actual_class
            if variant=="wrong_abi":
                body=body.replace("virtual int __cdecl AdvanceFade()","virtual int AdvanceFade()")
            cpp=folder/(variant+".cpp");cpp.write_text(PREFIX+body+"\nHalFaderWipe hal_wipes[7];\n"+actual_index+SUFFIX)
            exe=folder/(variant+".exe");batch=folder/(variant+".cmd");log=folder/(variant+".log")
            batch.write_text(msvc_env.discover().batch_header()+
                f'cl /nologo /O2 /TC /c /Fo"{folder}/caller.obj" "{ROOT}/src/func_02018efc.c" >"{log}" 2>&1\r\n'
                'if errorlevel 1 exit /b 1\r\n'+
                f'cl /nologo /std:c++17 /O2 /EHsc /MT /I"{ROOT}/include" /Fo"{folder}/{variant}.obj" '
                f'/Fe"{exe}" "{cpp}" "{folder}/caller.obj" >>"{log}" 2>&1\r\n',encoding="ascii",newline="")
            result=subprocess.run(["cmd","/d","/c",str(batch)],cwd=ROOT,capture_output=True,text=True,timeout=90)
            if result.returncode:raise AssertionError(log.read_text(errors="replace")+result.stderr)
            cls.exes[variant]=exe

    def run_case(self,variant,case):
        return subprocess.run([str(self.exes[variant]),case],capture_output=True,text=True,
                              timeout=15,creationflags=subprocess.CREATE_NO_WINDOW)

    def test_actual_raw_and_qualified_calls_and_other_slots(self):
        result=self.run_case("actual","slots")
        self.assertEqual(result.returncode,0,result.stdout+result.stderr)
        self.assertEqual(result.stdout.strip(),"PASS actual_class_seven_objects")

    def test_actual_index_rejects_foreign_and_misaligned_addresses(self):
        result=self.run_case("actual","index")
        self.assertEqual(result.returncode,0,result.stdout+result.stderr)
        self.assertEqual(result.stdout.strip(),"PASS index")

    def test_old_member_abi_is_rejected_by_the_actual_caller(self):
        result=self.run_case("wrong_abi","slots")
        self.assertNotEqual(result.returncode,0,"old thiscall slot unexpectedly passed the actual C caller")


if __name__=="__main__":
    unittest.main()
