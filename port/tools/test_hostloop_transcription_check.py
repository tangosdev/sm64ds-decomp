"""Exercise the actual wide-conductor transcription gate and its failure paths."""
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import hostloop_transcription_check as H


class TranscriptionCheck(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.root = pathlib.Path(__file__).resolve().parents[2]
        cls.src = (cls.root / 'src/func_0203ea5c.c').read_text()
        cls.wide = (cls.root / 'port/hal/comms_conductor_wide.cpp').read_text()

    def reject(self, old, new):
        self.assertIn(old, self.wide)
        candidate = self.wide.replace(old, new, 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_real_candidate_matches_all_original_lines_and_externs(self):
        self.assertEqual(H.check_sources(self.src, self.wide),
                         {'lines': 406, 'externs': 60, 'diagnostic_helpers': 3})

    def test_cli_uses_its_actual_tree_even_from_another_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            result = subprocess.run([sys.executable, str(self.root / 'port/tools/hostloop_transcription_check.py')],
                                    cwd=directory, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('406 lines', result.stdout)
        self.assertIn('60 symbols', result.stdout)

    def test_comments_do_not_change_the_reviewed_helper_tokens(self):
        candidate = self.wide.replace('++n;', '/* observational count */ ++n;', 1)
        self.assertEqual(H.check_sources(self.src, candidate)['lines'], 406)

    def test_changed_wait_condition_is_rejected(self):
        self.reject('while ((sp8 == 0) && (sp4 != 0))', 'while ((sp8 == 0) && (sp4 > 1))')

    def test_changed_game_state_assignment_is_rejected(self):
        self.reject('data_020a0f04 = 0;', 'data_020a0f04 = 1;')

    def test_changed_game_callee_is_rejected(self):
        self.reject('data_020a0f10 = func_02040704(temp_r0_5);',
                    'data_020a0f10 = func_02040714(temp_r0_5);')

    def test_diagnostic_counter_cannot_become_a_state_write(self):
        self.reject('++vs7_turns;', '++data_020a0f04;')

    def test_diagnostic_counter_cannot_move_outside_the_loop(self):
        candidate = self.wide.replace('++vs7_turns;', '', 1)
        candidate = candidate.replace('while ((sp8 == 0) && (sp4 != 0))',
                                      '++vs7_turns;\n    while ((sp8 == 0) && (sp4 != 0))', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_duplicate_instrument_is_not_erased(self):
        self.reject('++vs7_with_data;', '++vs7_with_data;\n            ++vs7_with_data;')

    def test_changed_report_argument_is_rejected(self):
        self.reject('report_bound_expiry(vs7_bound, vs7_turns, vs7_with_data,',
                    'report_bound_expiry(vs7_bound = 0, vs7_turns, vs7_with_data,')

    def test_missing_report_is_rejected(self):
        self.reject('report_bound_expiry(vs7_bound, vs7_turns, vs7_with_data,\n                        host_ms() - vs7_t0);', '')

    def test_warning_prefix_is_not_a_blanket_exception(self):
        self.reject('warn_info_mode_armed_wide();', 'warn_info_mode_armed_wide_mutates_state();')

    def test_warning_cannot_move_to_another_game_branch(self):
        candidate = self.wide.replace('warn_info_mode_armed_wide();', '', 1)
        candidate = candidate.replace('data_020a0f30 = 1;',
                                      'warn_info_mode_armed_wide();\n        data_020a0f30 = 1;', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_local_silencer_cannot_move_into_a_game_branch(self):
        candidate = self.wide.replace('(void)temp_r0_7;', '', 1)
        candidate = candidate.replace('data_020a0f30 = 1;',
                                      '(void)temp_r0_7;\n        data_020a0f30 = 1;', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_extern_width_change_is_rejected(self):
        self.reject('extern u8 data_020a0f04;', 'extern u16 data_020a0f04;')

    def test_logging_helper_cannot_acquire_a_game_state_write(self):
        self.reject('std::fflush(stderr);', 'data_020a0f04 = 1;')

    def test_clock_helper_cannot_acquire_a_game_state_write(self):
        self.reject('return (unsigned)std::chrono::duration_cast',
                    'data_020a0f04 = 1;\n    return (unsigned)std::chrono::duration_cast')

    def test_warning_helper_cannot_acquire_a_game_state_write(self):
        self.reject('said = true;', 'said = true;\n    data_020a0f04 = 1;')

    def test_continued_comment_cannot_hide_a_real_statement(self):
        self.reject('data_020a0f30 = 1;', '// removed by line splicing \\\n        data_020a0f30 = 1;')

    def test_missing_or_repeated_function_fails_closed(self):
        for candidate in (self.wide.replace('void conductor_wide(void)', 'void other(void)', 1),
                          self.wide + '\nvoid conductor_wide(void) {}'):
            with self.subTest(candidate=candidate[-70:]):
                with self.assertRaises(ValueError):
                    H.check_sources(self.src, candidate)


if __name__ == '__main__':
    unittest.main()
