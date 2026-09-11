"""Verify minigame graphics forwarding through the actual x86 source bodies."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hostgen
import msvc_env

ROOT = Path(__file__).resolve().parents[2]
SYMBOLS = ("func_ov004_020ae03c", "func_ov004_020ae06c",
           "func_ov004_020ae0a4", "func_ov004_020ae0d4")
HARNESS = r'''

#include <cstdio>
#include <cstring>
extern "C" {
int func_ov004_020ae03c(char*);int func_ov004_020ae06c(char*);int func_ov004_020ae0a4(char*);int func_ov004_020ae0d4(char*);
int func_ov004_020ae128(void*);
}
static unsigned hits[36];
#define MG_SLOT(n) (++hits[n])
ACTUAL_MB25
void*expected;void*seen;int slot_seen;
int __fastcall s22(void*s,void*){seen=s;slot_seen=22;return 122;}
int __fastcall s23(void*s,void*){seen=s;slot_seen=23;return 123;}
int __fastcall s24(void*s,void*){seen=s;slot_seen=24;return 124;}
int __fastcall s25(void*s,void*d){seen=s;slot_seen=25;return s==expected?mb_v25(s,d):-9;}

extern "C" {void *data_0209d4a8=nullptr;unsigned char data_0209d464=0;void func_02019100();}
static_assert(sizeof(void*)==4,"x86");
int main(){
 alignas(8) char scenes[2][0x4700]={};void*vt[36]={};
 vt[22]=(void*)s22;vt[23]=(void*)s23;vt[24]=(void*)s24;vt[25]=(void*)s25;
 int(*fn[])(char*)={func_ov004_020ae03c,func_ov004_020ae06c,func_ov004_020ae0a4,func_ov004_020ae0d4};
 int slots[]={25,24,22,23};unsigned failures=0;
 void*outer_vt[4]={nullptr,nullptr,nullptr,(void*)func_ov004_020ae03c};
 for(int round=0;round<32;++round){
  char*scene=scenes[round%2];*(void***)scene=vt;expected=scene;void*block[2]={outer_vt,scene};
  for(int i=0;i<4;++i){
   seen=nullptr;slot_seen=0;unsigned before,after;
   __asm { mov before,esp }
   __asm { mov ecx,3 }
   int result=fn[i]((char*)block);
   __asm { mov after,esp }
   if(seen!=expected||slot_seen!=slots[i]||result!=(slots[i]==24?124:1)||before!=after)++failures;
   block[1]=nullptr;seen=nullptr;slot_seen=0;
   __asm { mov before,esp }
   result=fn[i]((char*)block);
   __asm { mov after,esp }
   if(result!=1||slot_seen!=0||before!=after)++failures;
   block[1]=scene;
  }
  data_0209d4a8=block;data_0209d464=1;seen=nullptr;slot_seen=0;
  unsigned before,after;
  __asm { mov before,esp }
  __asm { mov ecx,3 }
  func_02019100();
  __asm { mov after,esp }
  if(seen!=expected||slot_seen!=25||data_0209d464!=0||before!=after)++failures;
 }
 data_0209d4a8=nullptr;data_0209d464=1;slot_seen=0;func_02019100();
 if(data_0209d464!=0||slot_seen!=0)++failures;
 if(hits[25]!=64)++failures;
 printf("forwarders=128 null_paths=128 outer_calls=32 actual_slot25=%u failures=%u\n",hits[25],failures);
 return failures?1:0;
}
'''


class ReviewedInputs(unittest.TestCase):
    def test_changed_calls_and_inactive_witnesses_refuse_before_emission(self):
        for symbol in SYMBOLS:
            source = (ROOT / "src" / (symbol + ".c")).read_text(encoding="utf-8")
            old = "return vfn(r0);" if symbol.endswith("06c") else "fn(p);"
            self.assertEqual(source.count(old), 1)
            variants = ("(void)0;", old + old,
                        "/*" + old + "*/(void)0;",
                        "#if 0\n" + old + "\n#endif\n(void)0;")
            for replacement in variants:
                with self.subTest(symbol=symbol, replacement=replacement), tempfile.TemporaryDirectory() as temp:
                    root = Path(temp)
                    path = root / "src" / (symbol + ".c")
                    path.parent.mkdir()
                    path.write_text(source.replace(old, replacement), encoding="utf-8")
                    output = root / "output"
                    with self.assertRaisesRegex(SystemExit, "reviewed ABI source changed"):
                        hostgen.emit(path, output, root)
                    self.assertFalse(output.exists())

    def test_crlf_inputs_emit_the_same_source(self):
        for symbol in SYMBOLS:
            with self.subTest(symbol=symbol), tempfile.TemporaryDirectory() as temp:
                root = Path(temp)
                path = root / "src" / (symbol + ".c")
                path.parent.mkdir()
                source = (ROOT / "src" / (symbol + ".c")).read_text(encoding="utf-8")
                path.write_bytes(source.encode("utf-8"))
                lf, _ = hostgen.emit(path, root / "lf", root)
                path.write_bytes(source.replace("\n", "\r\n").encode("utf-8"))
                crlf, _ = hostgen.emit(path, root / "crlf", root)
                self.assertEqual(lf.read_bytes(), crlf.read_bytes())


@unittest.skipUnless(os.name == "nt", "the production port uses Windows x86")
class GraphicsABI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        (ROOT / "build").mkdir(exist_ok=True)
        cls.scratch = tempfile.TemporaryDirectory(prefix="mg-graphics-abi-", dir=ROOT / "build")
        cls.addClassCleanup(cls.scratch.cleanup)
        folder = Path(cls.scratch.name)
        lines = [line for line in (ROOT / "port/hal/scene_mg.cpp").read_text(encoding="utf-8").splitlines()
                 if line.startswith("static int  __fastcall mb_v25")]
        if len(lines) != 1:
            raise AssertionError("re-derive the actual slot-25 face before testing")
        harness = folder / "probe.cpp"
        harness.write_text(HARNESS.replace("ACTUAL_MB25", lines[0]), encoding="utf-8")
        cls.exes = {}
        for mode in ("raw", "adapted"):
            build = folder / mode
            build.mkdir()
            sources = []
            for symbol in SYMBOLS:
                source = ROOT / "src" / (symbol + ".c")
                if mode == "adapted":
                    emitted, _ = hostgen.emit(source, build / "generated", ROOT)
                else:
                    emitted = build / (symbol + ".cpp")
                    emitted.write_text('extern "C" {\n' + source.read_text(encoding="utf-8") + '\n}\n', encoding="utf-8")
                sources.append(emitted)
            objects = []
            command = msvc_env.discover().batch_header()
            for symbol in ("func_ov004_020ae128", "func_02019100"):
                obj = build / (symbol + ".obj")
                objects.append(obj)
                command += (f'cl /nologo /O2 /MT /TC /c /I"{ROOT}/include" '
                            f'/Fo"{obj}" "{ROOT}/src/{symbol}.c" >>"{build}/compile.log" 2>&1\r\n'
                            'if errorlevel 1 exit /b 1\r\n')
            exe = build / "probe.exe"
            command += (f'cl /nologo /O2 /EHsc /MT /I"{ROOT}/port" /I"{ROOT}/port/ntr/include" /I"{ROOT}/include" '
                        f'/Fe"{exe}" /Fo"{build}/" "{harness}" ' +
                        ' '.join('"' + str(path) + '"' for path in sources + objects) +
                        f' >>"{build}/compile.log" 2>&1\r\n')
            batch = build / "build.cmd"
            batch.write_text(command, encoding="ascii", newline="")
            result = subprocess.run(["cmd", "/d", "/c", str(batch)], capture_output=True, text=True, timeout=120)
            if result.returncode:
                raise AssertionError((build / "compile.log").read_text(errors="replace"))
            cls.exes[mode] = exe

    def test_all_forwarders_and_actual_outer_dispatch_preserve_contracts(self):
        result = subprocess.run([str(self.exes["adapted"])], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("actual_slot25=64 failures=0", result.stdout)

    def test_raw_call_conventions_fail_the_same_actual_source_control(self):
        result = subprocess.run([str(self.exes["raw"])], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertNotIn("failures=0", result.stdout)


if __name__ == "__main__":
    unittest.main()
