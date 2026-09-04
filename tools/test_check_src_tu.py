"""Regression tests for tools/check_src_tu.py.

A gate's failure branch is the one path a green CI run never exercises, so each of
these plants the breakage and asserts the gate goes red -- including the empty work
list, which this tree has repeatedly shipped as a silent pass.
"""
import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import check_src_tu as C  # noqa: E402

# A real ROM symbol and a real header, so the happy path is not synthetic.
GOOD_SYMBOL = "_ZN10dBgW_KcMbgD1Ev"
GOOD_HEADER = "decl_dBgW_KcMbg.h"
# What #1643 left behind.
RETIRED_SYMBOL = "_ZN18MovingMeshColliderD1Ev"
RETIRED_HEADER = "decl_MovingMeshCollider.h"


class Scratch:
    """A throwaway TU directory INSIDE the repo -- check() reports paths relative to
    the repo root, so a tempdir elsewhere on the filesystem would not be expressible."""

    def __enter__(self):
        (REPO / "build").mkdir(exist_ok=True)
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="check_src_tu_", dir=REPO / "build"))
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)

    def write(self, name, body):
        (self.dir / name).write_text(body, encoding="utf-8")
        return self

    def check(self):
        return C.check(self.dir)


def failures_of(report, kind):
    return [f for f in report["failures"] if f["check"] == kind]


class TreeTests(unittest.TestCase):
    def test_the_committed_tree_passes(self):
        """If this is red, src_tu is stranded -- fix the tree, not the test."""
        report = C.check(REPO / "src_tu")
        self.assertTrue(report["ok"], report["failures"][:10])
        # A smoke floor, not a target: src_tu only ever SHRINKS, because every TU
        # promotion consolidates its shadow source into src/. The number here says
        # "the check walked a real tree", so it has to stay below the live count --
        # it was 30 when this promotion took the count to exactly 30.
        self.assertGreater(report["checked"]["sources"], 10)
        self.assertGreater(report["checked"]["references"], 100)

    def test_cli_exits_zero_on_the_committed_tree(self):
        r = subprocess.run([sys.executable, str(TOOLS / "check_src_tu.py")],
                           capture_output=True, text=True, cwd=REPO)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)


class IncludeTests(unittest.TestCase):
    def test_a_resolvable_include_passes(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\nint f(){{return 0;}}\n'
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertTrue(report["ok"], report["failures"])

    def test_a_stranded_include_fails(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{RETIRED_HEADER}"\n'
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "includes")), 1)

    def test_an_include_beside_the_source_resolves(self):
        with Scratch() as s:
            s.write("local.h", "/* here */\n")
            report = s.write("t.cpp", '#include "local.h"\n'
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertTrue(report["ok"], report["failures"])

    def test_angle_includes_are_out_of_scope(self):
        with Scratch() as s:
            report = s.write("t.cpp", "#include <stdlib.h>\n"
                                      "#include <MovingMeshCollider.h>\n"
                                      f'#include "{GOOD_HEADER}"\n'
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertTrue(report["ok"], report["failures"])


class SymbolTests(unittest.TestCase):
    def test_a_retired_symbol_fails(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      f"void g(){{{RETIRED_SYMBOL}();}}\n").check()
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "symbols")), 1)

    def test_a_stale_length_prefix_fails(self):
        """The #1643 trap the brief names: 18 -> 10 because the identifier shortens.
        Keeping the old length produces a symbol that links to nothing."""
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      "void g(){_ZN18dBgW_KcMbgD1Ev();}\n").check()
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "symbols")), 1)

    def test_a_symbol_named_only_in_a_comment_is_prose_not_a_reference(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      f"/* {RETIRED_SYMBOL} is what this used to call */\n"
                                      f"// and so is {RETIRED_SYMBOL}\n"
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertTrue(report["ok"], report["failures"])

    def test_a_symbol_named_only_in_a_string_literal_is_not_a_reference(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      f'const char *n = "{RETIRED_SYMBOL}";\n'
                                      f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertTrue(report["ok"], report["failures"])

    def test_a_compiler_emitted_variant_of_a_real_class_is_forgiven(self):
        """D2 and the vague-linkage _ZTV/_ZTI/_ZTS are minted by the TU itself and are
        in no symbols.txt; forgiving them is what keeps this gate free of noise."""
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      "void g(){_ZN8PoleLiftD2Ev(); (void)_ZTI8PoleLift;"
                                      " (void)_ZTV8PoleLift; (void)_ZTS8PoleLift;}\n").check()
            self.assertTrue(report["ok"], report["failures"])

    def test_a_compiler_emitted_variant_of_a_class_the_rom_never_names_fails(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\n'
                                      "void g(){(void)_ZTV18MovingMeshCollider;}\n").check()
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "symbols")), 1)


class EmptyWorkListTests(unittest.TestCase):
    """An empty check is not a pass -- check_header_offsets.py's own docstring argues
    this, and check_references.py's "skipping" is the counterexample."""

    def test_no_translation_units_fails(self):
        with Scratch() as s:
            report = s.check()
            self.assertFalse(report["ok"])
            self.assertEqual(len(failures_of(report, "worklist")), 1)

    def test_sources_with_no_includes_fails(self):
        with Scratch() as s:
            report = s.write("t.cpp", f"void g(){{{GOOD_SYMBOL}();}}\n").check()
            self.assertFalse(report["ok"])
            self.assertTrue(any("zero #include" in f["message"]
                                for f in failures_of(report, "worklist")))

    def test_sources_with_no_references_fails(self):
        with Scratch() as s:
            report = s.write("t.cpp", f'#include "{GOOD_HEADER}"\nint f(){{return 0;}}\n').check()
            self.assertFalse(report["ok"])
            self.assertTrue(any("zero mangled" in f["message"]
                                for f in failures_of(report, "worklist")))

    def test_cli_exits_one_on_an_empty_root(self):
        with Scratch() as s:
            rel = s.dir.relative_to(REPO).as_posix()
            r = subprocess.run([sys.executable, str(TOOLS / "check_src_tu.py"), "--root", rel],
                               capture_output=True, text=True, cwd=REPO)
            self.assertEqual(r.returncode, 1, r.stdout + r.stderr)


if __name__ == "__main__":
    unittest.main()
