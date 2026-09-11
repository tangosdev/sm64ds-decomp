"""Check the whole Chuckya host Behavior and the real ModelAnim callee on x86."""
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
SYMBOL = "_ZN7Chuckya8BehaviorEv"
SOURCE = ROOT / "src" / (SYMBOL + ".cpp")
RAW_CALL = "(*(void(**)(void*))(*(int*)o + 0xc))(o);"

HARNESS = r'''
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "ModelAnim.h"
const void *expected_model,*seen_model,*seen_components;BCA_File *seen_file;
int seen_frame,slot_calls,verts_calls,early_exit;
void ModelComponents::UpdateBones(BCA_File *file,int frame){seen_components=this;seen_file=file;seen_frame=frame;}
void ModelComponents::UpdateVertsUsingBones(){++verts_calls;}
void __fastcall update_slot(void *self,void *) {seen_model=self;++slot_calls;if(self==expected_model)((ModelAnim*)self)->ModelAnim::UpdateVerts();}
extern "C" {
int _ZN7Chuckya8BehaviorEv(char*);
signed char data_0209f2f8=0;
unsigned char data_ov062_0211dea0[16]={},data_ov062_0211dec0[16]={},data_ov062_0211de70[16]={},data_ov062_0211ded0[16]={},data_ov062_0211dee0[16]={},data_ov062_0211de90[16]={},data_ov062_0211df00[16]={};
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void*,void*,void*,unsigned){return early_exit;}
unsigned short DecIfAbove0_Short(unsigned short*p){return *p;}
void _ZN5Actor9UpdatePosEP12CylinderClsn(void*,void*){}
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void*,void*,int,short,int,int,void*){return 0;}
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*,void*,unsigned){}
void func_ov062_02116010(void*){}
void _ZN12CylinderClsn5ClearEv(void*){}
void _ZN12CylinderClsn6UpdateEv(void*){}
void _ZN9Animation7AdvanceEv(void*){}
void func_ov062_02116d28(void*){}
void func_ov062_02116e80(void*){}
void func_ov062_02116dbc(void*){}
}
static_assert(sizeof(void*)==4,"x86 only");
int main(){
 alignas(8) char actor[0x500]={};void *table[7]={};table[3]=(void*)&update_slot;
 ModelAnim *model=(ModelAnim*)(actor+0x300);expected_model=model;*(void***)model=table;
 model->file=(BCA_File*)0x12345678;model->currFrame=0x9000;
 *(void**)(actor+0x364)=data_ov062_0211dea0;
 unsigned before,after;
 __asm {mov before,esp}
 int result=_ZN7Chuckya8BehaviorEv(actor);
 __asm {mov after,esp}
 bool ok=result==1&&before==after&&seen_model==expected_model&&slot_calls==1&&
         seen_components==&model->data&&seen_file==model->file&&seen_frame==9&&verts_calls==1;
 printf("receiver=%d components=%d frame=%d verts=%d stack=%d result=%d\n",seen_model==expected_model,seen_components==&model->data,seen_frame,verts_calls,before==after,result);
 early_exit=1;slot_calls=0;result=_ZN7Chuckya8BehaviorEv(actor);ok=ok&&result==1&&slot_calls==0;
 printf("early_exit=%d slot_calls=%d\n",result,slot_calls);return ok?0:1;
}
'''


class GeneratorDrift(unittest.TestCase):
    def test_missing_changed_or_duplicate_call_refuses_output(self):
        source = SOURCE.read_text(encoding="utf-8")
        for replacement in ("(void)o;", RAW_CALL.replace("0xc", "0x10"), RAW_CALL + RAW_CALL):
            with self.subTest(replacement=replacement), tempfile.TemporaryDirectory() as temp:
                root = Path(temp)
                path = root / "src" / (SYMBOL + ".cpp")
                path.parent.mkdir()
                path.write_text(source.replace(RAW_CALL, replacement), encoding="utf-8")
                output = root / "output"
                with self.assertRaisesRegex(SystemExit, "reviewed ABI source changed"):
                    hostgen.emit(path, output, root)
                self.assertFalse(output.exists(), "source drift must not produce a candidate")

    def test_inactive_decoys_and_unreviewed_context_refuse_output(self):
        source = SOURCE.read_text(encoding="utf-8")
        changed = RAW_CALL.replace("0xc", "0x10")
        variants = [
            source.replace(RAW_CALL, "/*" + RAW_CALL + "*/" + changed),
            source.replace(RAW_CALL, "/*" + RAW_CALL + "*/(void)o;"),
            source.replace(RAW_CALL, "#if 0\n" + RAW_CALL + "\n#endif\n" + changed),
            source + "\n// unreviewed context\n",
        ]
        for index, text in enumerate(variants):
            with self.subTest(index=index), tempfile.TemporaryDirectory() as temp:
                root = Path(temp)
                path = root / "src" / (SYMBOL + ".cpp")
                path.parent.mkdir()
                path.write_text(text, encoding="utf-8")
                output = root / "output"
                with self.assertRaisesRegex(SystemExit, "reviewed ABI source changed"):
                    hostgen.emit(path, output, root)
                self.assertFalse(output.exists())

    def test_line_endings_preserve_the_reviewed_generation(self):
        source = SOURCE.read_text(encoding="utf-8")
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            path = root / "src" / (SYMBOL + ".cpp")
            path.parent.mkdir()
            emitted = []
            for index, newline in enumerate(("\n", "\r\n", "\r")):
                path.write_bytes(source.replace("\n", newline).encode("utf-8"))
                result, _ = hostgen.emit(path, root / str(index), root)
                emitted.append(result.read_bytes())
            self.assertEqual(emitted[0], emitted[1])
            self.assertEqual(emitted[0], emitted[2])


@unittest.skipUnless(os.name == "nt", "the production port uses Windows x86")
class ChuckyaModelABI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        (ROOT / "build").mkdir(exist_ok=True)
        cls.scratch = tempfile.TemporaryDirectory(prefix="chuckya-abi-test-", dir=ROOT / "build")
        cls.addClassCleanup(cls.scratch.cleanup)
        folder = Path(cls.scratch.name)
        emitted, _ = hostgen.emit(SOURCE, folder / "generated", ROOT)
        harness = folder / "probe.cpp"
        harness.write_text(HARNESS, encoding="utf-8")
        cls.exes = {}
        for variant, candidate in (("actual", emitted), ("old_raw_call", SOURCE)):
            exe = folder / (variant + ".exe")
            batch = folder / (variant + ".cmd")
            log = folder / (variant + ".log")
            batch.write_text(msvc_env.discover().batch_header()
                + f'cl /nologo /std:c++17 /O2 /EHsc /MT /vmg /vmm '
                  f'/I"{ROOT}/include" /I"{ROOT}/port/ntr/include" /Fo"{folder}/" '
                  f'/Fe"{exe}" "{candidate}" "{harness}" '
                  f'"{ROOT}/src/_ZN9ModelAnim11UpdateVertsEv.cpp" >"{log}" 2>&1\r\n',
                encoding="ascii", newline="")
            result = subprocess.run(["cmd", "/d", "/c", str(batch)], cwd=ROOT,
                                    capture_output=True, text=True, timeout=90)
            if result.returncode:
                raise AssertionError(log.read_text(errors="replace") + result.stderr)
            cls.exes[variant] = exe

    def run_case(self, variant):
        return subprocess.run([str(self.exes[variant])], capture_output=True, text=True,
                              timeout=15, creationflags=subprocess.CREATE_NO_WINDOW)

    def test_actual_behavior_reaches_actual_model_method_with_correct_receiver(self):
        result = self.run_case("actual")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("receiver=1 components=1 frame=9 verts=1 stack=1 result=1", result.stdout)
        self.assertIn("early_exit=1 slot_calls=0", result.stdout)

    def test_old_raw_call_keeps_the_wrong_receiver(self):
        result = self.run_case("old_raw_call")
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("receiver=0 components=0 frame=0 verts=0 stack=1 result=1", result.stdout)


if __name__ == "__main__":
    unittest.main()
