import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import mangle as MG  # noqa: E402


ROWS = [
    MG.EmittedSymbol(".text", "_ZN5Actor8BehaviorEv", "FUNC", "GLOBAL", 0x20),
    MG.EmittedSymbol(".data", "plainData", "OBJECT", "WEAK", 4),
]


class MangleTests(unittest.TestCase):
    def run_main(self, *args):
        stdout = io.StringIO()
        stderr = io.StringIO()
        with tempfile.TemporaryDirectory() as tmp:
            source = pathlib.Path(tmp) / "candidate.cpp"
            source.write_text(
                "struct Actor { void Behavior(); };\n"
                "void Actor::Behavior() {}\n",
                encoding="utf-8",
            )
            with mock.patch.object(MG.M, "compile_c", return_value=b"object"), \
                    mock.patch.object(MG, "defined_symbols", return_value=ROWS), \
                    contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
                code = MG.main([str(source), *args])
        return code, stdout.getvalue(), stderr.getvalue()

    def test_expect_accepts_exact_compiler_symbol(self):
        code, output, error = self.run_main(
            "--expect", "_ZN5Actor8BehaviorEv", "--mangled-only"
        )
        self.assertEqual(code, 0)
        self.assertIn("_ZN5Actor8BehaviorEv", output)
        self.assertNotIn("plainData", output)
        self.assertEqual(error, "")

    def test_expect_rejects_a_different_signature(self):
        code, _output, error = self.run_main("--expect", "_ZN5Actor8BehaviorEi")
        self.assertEqual(code, 1)
        self.assertIn("_ZN5Actor8BehaviorEi", error)

    def test_json_is_machine_readable(self):
        code, output, _error = self.run_main("--json")
        document = json.loads(output)
        self.assertEqual(code, 0)
        self.assertEqual(document["version"], "2004/b56")
        self.assertEqual(document["symbols"][0]["name"], "_ZN5Actor8BehaviorEv")
        self.assertEqual(document["compiler_output"], [])


if __name__ == "__main__":
    unittest.main()
