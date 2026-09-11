"""Installed-tool discovery and actual shipping-script integration tests."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import types
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import msvc_env as M
import battery
import closure


class DiscoveryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.env = {'ProgramFiles(x86)': str(self.root / 'PF'), 'PATH': 'fixture-path'}
        self.vs = self.root / 'Visual Studio/18/Community'
        self.vcvars = self.touch(self.vs / 'VC/Auxiliary/Build/vcvars32.bat')
        self.vswhere = self.touch(self.root / 'PF/Microsoft Visual Studio/Installer/vswhere.exe')
        self.cmake = self.touch(self.root / 'standalone/cmake.exe')
        self.ninja = self.touch(self.root / 'standalone/ninja.exe')
        self.run = mock.Mock(return_value=types.SimpleNamespace(returncode=0, stdout=str(self.vs) + '\n'))
        self.which = mock.Mock(side_effect=lambda name, path: {'cmake': str(self.cmake), 'ninja': str(self.ninja)}.get(name))

    def touch(self, path):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('fixture')
        return path

    def discover(self):
        return M.discover(self.env, self.run, self.which)

    def test_vs18_community_and_standalone_tools(self):
        result = self.discover()
        self.assertEqual(result, M.Toolchain(self.vcvars.resolve(), self.cmake.resolve(), self.ninja.resolve()))
        self.assertIn('Microsoft.VisualStudio.Component.VC.Tools.x86.x64', self.run.call_args.args[0])
        self.assertEqual(self.run.call_args.kwargs['timeout'], 15)
        self.which.assert_any_call('ninja', path='fixture-path')

    def test_existing_developer_installation_is_used(self):
        other = self.root / 'Other VS'
        expected = self.touch(other / 'VC/Auxiliary/Build/vcvars32.bat')
        self.env['VSINSTALLDIR'] = str(other)
        self.assertEqual(self.discover().vcvars, expected.resolve())

    def test_historical_buildtools_installation_remains_supported(self):
        expected = self.touch(self.root / 'PF/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvars32.bat')
        self.assertEqual(self.discover().vcvars, expected.resolve())

    def test_bundled_tools_take_precedence_when_present(self):
        cm = self.vs / 'Common7/IDE/CommonExtensions/Microsoft/CMake'
        expected_cmake = self.touch(cm / 'CMake/bin/cmake.exe')
        expected_ninja = self.touch(cm / 'Ninja/ninja.exe')
        result = self.discover()
        self.assertEqual((result.cmake, result.ninja), (expected_cmake.resolve(), expected_ninja.resolve()))
        self.which.assert_not_called()

    def test_missing_tool_or_compiler_refuses(self):
        self.which.return_value = None
        self.which.side_effect = None
        with self.assertRaisesRegex(M.ToolchainError, 'cmake'):
            self.discover()
        self.vcvars.unlink()
        with self.assertRaisesRegex(M.ToolchainError, 'x86 C\+\+'):
            self.discover()

    def test_invalid_vswhere_responses_refuse(self):
        for result in (types.SimpleNamespace(returncode=1, stdout=''),
                       types.SimpleNamespace(returncode=0, stdout='missing'),
                       types.SimpleNamespace(returncode=0, stdout=str(self.vs) + '\nsecond')):
            with self.subTest(result=result):
                self.run.return_value = result
                with self.assertRaises(M.ToolchainError):
                    self.discover()

    def test_vswhere_timeout_and_os_error_refuse(self):
        for error in (subprocess.TimeoutExpired('vswhere', 15), OSError('cannot start')):
            with self.subTest(error=error):
                self.run.side_effect = error
                with self.assertRaisesRegex(M.ToolchainError, 'discovery failed'):
                    self.discover()

    def test_missing_vswhere_can_use_existing_installation(self):
        self.vswhere.unlink()
        self.env['VSINSTALLDIR'] = str(self.vs)
        self.assertEqual(self.discover().vcvars, self.vcvars.resolve())
        self.run.assert_not_called()

    def test_closure_needs_only_vcvars_not_cmake_or_ninja(self):
        with mock.patch.object(M, 'discover_vcvars', return_value=self.vcvars), mock.patch.object(M, 'discover', side_effect=AssertionError('build tools not needed')):
            self.assertEqual(closure.find_vcvars(), str(self.vcvars))
        with mock.patch.object(M, 'discover_vcvars', side_effect=M.ToolchainError('no compiler')):
            self.assertIsNone(closure.find_vcvars())

    def test_batch_header_uses_exact_tools_and_propagates_vcvars_failure(self):
        result = self.discover()
        header = result.batch_header()
        self.assertIn('call "%s" >nul\r\nif errorlevel 1 exit /b 1' % result.vcvars, header)
        self.assertIn('set "PORT_CMAKE=%s"' % result.cmake, header)
        self.assertIn('set "PORT_NINJA=%s"' % result.ninja, header)

    def test_batch_metacharacters_are_refused(self):
        for value in ('bad%PATH%', 'bad!value!', 'bad&command', 'bad\ncommand'):
            with self.subTest(value=value):
                with self.assertRaises(M.ToolchainError):
                    M.batch_path(self.root / value)

    def test_real_shipping_script_preserves_enforced_configuration(self):
        build = self.root / 'build/port-kit'
        build.mkdir(parents=True)
        with mock.patch.object(M, 'discover', return_value=self.discover()):
            script, error, decision, detail = battery.shipcfg_script(str(self.root), str(build))
        self.assertIsNone(error)
        self.assertEqual(decision, 'refused')
        contents = Path(script).read_text()
        for name, value in battery.SHIPCFG_CACHE_TABLE:
            self.assertIn('-D%s=%s' % (name, value), contents)
        self.assertIn('"%s" -S' % self.cmake.resolve(), contents)
        self.assertIn('-DCMAKE_MAKE_PROGRAM="%s"' % self.ninja.resolve(), contents)
        self.assertIn('"%s" -C "%s" walk_window' % (self.ninja.resolve(), build), contents)
        self.assertNotIn('if not exist', contents)
        (build / 'build.ninja').write_text('fixture')
        entries = list(battery.SHIPCFG_CACHE_TABLE) + [('CMAKE_GENERATOR', battery.SHIPCFG_GENERATOR)]
        (build / 'CMakeCache.txt').write_text('\n'.join('%s:STRING=%s' % row for row in entries))
        with mock.patch.object(M, 'discover', return_value=self.discover()):
            script, error, decision, detail = battery.shipcfg_script(str(self.root), str(build))
        self.assertEqual(decision, 'fast path')
        self.assertIn('if not exist', Path(script).read_text())
        (build / 'CMakeCache.txt').write_text('CMAKE_BUILD_TYPE:STRING=Debug\n')
        with mock.patch.object(M, 'discover', return_value=self.discover()):
            script, error, decision, detail = battery.shipcfg_script(str(self.root), str(build))
        self.assertEqual(decision, 'refused')
        self.assertNotIn('if not exist', Path(script).read_text())

    def test_missing_setup_produces_no_shipping_script(self):
        with mock.patch.object(M, 'discover', side_effect=M.ToolchainError('missing Ninja')):
            script, error, decision, detail = battery.shipcfg_script(str(self.root), str(self.root / 'kit'))
        self.assertIsNone(script)
        self.assertEqual(error, 'missing Ninja')
        self.assertEqual((decision, detail), (None, None))


if __name__ == '__main__':
    unittest.main()
