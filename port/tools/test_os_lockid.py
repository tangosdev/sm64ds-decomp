"""Compile the actual host allocator and exercise its x86 initialization contract."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import msvc_env

ROOT = Path(__file__).resolve().parents[2]
HARNESS = r'''
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>
#include "ACTUAL_SOURCE"
static_assert(sizeof(void *) == 4, "The host port uses x86 addresses");

static int expect(bool yes, const char *message) {
    if (!yes) { std::fprintf(stderr, "%s\n", message); return 1; }
    return 0;
}
int main(int argc, char **argv) {
    if (argc != 2) return 2;
    if (VirtualAlloc((void *)0x027f0000u, 0x10000, MEM_RESERVE | MEM_COMMIT,
                     PAGE_READWRITE) != (void *)0x027f0000u) return 3;
    volatile unsigned *words = (volatile unsigned *)0x027fffb0u;
    if (!std::strcmp(argv[1], "early")) {
        if (expect(func_02057020() == -3, "zeroed words must report no free ID")) return 1;
        port_os_lock_words_seed();
        if (expect(func_02057020() == 0x40, "early FS allocation must get the first ID")) return 1;
    } else if (!std::strcmp(argv[1], "boot")) {
        port_os_lock_words_seed();
        if (expect(func_02057020() == 0x40, "first allocation")) return 1;
        port_os_lock_words_seed();
        if (expect(words[0] == 0x7fffffffu && words[1] == 0xffff0000u,
                   "later boot must preserve allocated bits")) return 1;
        if (expect(func_02057020() == 0x41, "later boot must not recycle the FS ID")) return 1;
    } else if (!std::strcmp(argv[1], "exhaustion")) {
        port_os_lock_words_seed();
        for (int i = 0; i < 48; ++i)
            if (expect(func_02057020() == 0x40 + i, "ID sequence or reserved range changed")) return 1;
        if (expect(func_02057020() == -3, "exhaustion sentinel changed")) return 1;
        port_os_lock_words_seed();
        if (expect(words[0] == 0 && words[1] == 0 && func_02057020() == -3,
                   "exhaustion must not trigger reseeding")) return 1;
    } else if (!std::strcmp(argv[1], "concurrent_seed")) {
        std::vector<std::thread> threads;
        for (int i=0; i<32; ++i) threads.emplace_back([] { port_os_lock_words_seed(); });
        for (auto &thread : threads) thread.join();
        if (expect(words[0] == 0xffffffffu && words[1] == 0xffff0000u,
                   "concurrent seed did not finish both words")) return 1;
        if (expect(func_02057020() == 0x40, "allocation after concurrent initialization")) return 1;
        threads.clear();
        for (int i=0; i<32; ++i) threads.emplace_back([] {
            for (int j=0; j<100; ++j) port_os_lock_words_seed();
        });
        for (auto &thread : threads) thread.join();
        if (expect(func_02057020() == 0x41, "repeated concurrent seeds recycled an ID")) return 1;
    } else return 2;
    std::printf("PASS %s\n", argv[1]);
    return 0;
}
'''


@unittest.skipUnless(os.name == "nt", "the production port uses Windows x86")
class LockWordInitialization(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        (ROOT / "build").mkdir(exist_ok=True)
        cls.scratch = tempfile.TemporaryDirectory(prefix="lockid-test-", dir=ROOT / "build")
        cls.addClassCleanup(cls.scratch.cleanup)
        directory = Path(cls.scratch.name)
        source = directory / "probe.cpp"
        source.write_text(HARNESS.replace("ACTUAL_SOURCE", (ROOT / "port/hal/os_lockid.cpp").as_posix()))
        cls.exe = directory / "probe.exe"
        batch = directory / "build.cmd"
        log = directory / "compile.log"
        batch.write_text(
            msvc_env.discover().batch_header()
            + f'cl /nologo /std:c++17 /O2 /EHsc /MT /Fo"{directory / "probe.obj"}" '
              f'/Fe"{cls.exe}" "{source}" >"{log}" 2>&1\r\n',
            encoding="ascii", newline="")
        result = subprocess.run(["cmd", "/d", "/c", str(batch)], cwd=ROOT,
                                capture_output=True, text=True, timeout=90)
        if result.returncode:
            raise AssertionError(log.read_text(errors="replace") + result.stderr)

    def check_case(self, name):
        result = subprocess.run([str(self.exe), name], capture_output=True,
                                text=True, timeout=20, creationflags=subprocess.CREATE_NO_WINDOW)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stdout.strip(), "PASS " + name)

    def test_early_filesystem_allocation(self):
        self.check_case("early")

    def test_later_boot_preserves_allocated_ids(self):
        self.check_case("boot")

    def test_all_ids_then_exhaustion_remains_exhausted(self):
        self.check_case("exhaustion")

    def test_concurrent_seed_is_once_only(self):
        self.check_case("concurrent_seed")


if __name__ == "__main__":
    unittest.main()
