import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import affected_src as A


class AffectedSourcesTests(unittest.TestCase):
    def test_transitive_header_consumers_and_direct_sources(self):
        with tempfile.TemporaryDirectory() as td:
            repo = pathlib.Path(td)
            (repo / "include").mkdir()
            (repo / "src").mkdir()
            (repo / "include" / "types.h").write_text("typedef int s32;\n")
            (repo / "include" / "Timer.hpp").write_text('#include "types.h"\n')
            (repo / "src" / "timer.cpp").write_text('#include "Timer.hpp"\n')
            (repo / "src" / "plain.c").write_text("void plain(void) {}\n")

            self.assertEqual(
                A.affected_sources(["include/types.h"], repo),
                ["src/timer.cpp"],
            )
            self.assertEqual(
                A.affected_sources(["src/plain.c"], repo),
                ["src/plain.c"],
            )


if __name__ == "__main__":
    unittest.main()
