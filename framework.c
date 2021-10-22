#include "framework.h"
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

uint tests = 0, testok = 0, intest = 0, intestok = 0, allc = 0, allcok = 0;
const char *def = " -- NO TEST -- ";
const char *curr;

void start_test(const char *test) {
  tests++;
  curr = test;
  printf(BOLD BLUE " === TEST %s STARTED ===\n" RESET, test);
}

void cleanup(int code) {
  curr = def;
  if(intest == intestok) testok++;
  allc += intest; allcok += intestok;
  intest = 0;
  intestok = 0;
  if(code != 0) exit(code);
}

#define RGEQ(v1, v2) ((v1) == (v2) ? BOLD GREEN : BOLD RED)
void print_summary(bool glob) {
  if(glob) {
    float frac_tst = (float)testok / (float)tests;
    const char *col_tst = (frac_tst < 0.75f) ? BRED : (frac_tst >= 1.0f) ? GREEN : BYELLOW;
    float frac_glob = (float)allcok / (float)allc;
    const char *col_glob = (frac_glob < 0.75f) ? BRED : (frac_glob >= 1.0f) ? GREEN : BYELLOW;
    printf(BOLD
      "\t| Tests ran            | %6d |\n"
      "\t| Tests succeeded      | %s%6d%s |\n"
      "\t| Test success ratio   | %s%.4f%s |\n"
      "\t| Checks ran           | %6d |\n"
      "\t| Checks succeeded     | %s%6d%s |\n"
      "\t| Global success ratio | %s%.4f%s |\n"
      "\t+----------------------+--------+\n" RESET,
      tests, RGEQ(tests, testok), testok, RESET BOLD, col_tst, frac_tst, RESET BOLD,
      allc, RGEQ(allc, allcok), allcok, RESET BOLD, col_glob, frac_glob, RESET BOLD
    );
  }
  else {
    printf(BOLD
      "  Checks ran:    %d\n"
      "  Checks passed: %s%d%s\n"
      "  Success ratio: %s%f\n" RESET,
      intest, RGEQ(intest, intestok), intestok, RESET BOLD, RGEQ(intest, intestok), (float)intestok / (float)intest
    );
  }
}

void abort_test() {
  print_summary(FALSE);
  fprintf(stderr,
    RED " [ Failure: Fatal error during test. ]\n\n" RESET
    BOLD BLUE " === TEST %s FINISHED ===\n" RESET,
    curr
  );
  cleanup(-1);
}

void end_test() {
  print_summary(FALSE);
  printf(
    RESET " [ End of Test: All tests ran. ]\n"
    BOLD BLUE " === TEST %s FINISHED ===\n\n" RESET,
    curr
  );
  cleanup(0);
}

void checks() {
  intest++;
}

void success() {
  intestok++;
}

void caught(int signal) {

  const char *_sig = "SIG_UNKNOWN";

  switch(signal) {
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define X(s_new, s_old) case s_old: _sig = STR(s_new); break;
XSIGNALS
#undef X
  }

  abort_test();
  fprintf(stderr, RED BOLD
    "\t+-------------------------------+\n"
    "\t|       TESTS INTERRUPTED       |\n"
    "\t+-------------------------------+\n"
    "\t  -> Signal %s (%d) was thrown.\n" RESET,
    _sig, signal
  );
  exit(signal);
}

const char *_str_sgl(sig_e sgl) {
  switch(sgl) {
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define X(s_n, s_o) case s_n: return STR(s_n) "/" #s_o;
XSIGNALS
#undef X
  }
}

void catch_sig(sig_e sgl) {
  if(sgl == SIG_CATCHALL) {
    catch_sig(SIG_TERMINATED); catch_sig(SIG_ABORTED); catch_sig(SIG_FLOATERR);
    catch_sig(SIG_SEGMFAULT); catch_sig(SIG_INTERRUPT); catch_sig(SIG_INSTRUCT);
    return;
  }

  fprintf(stderr, BOLD BLUE " [ Adding signal handler for %d (%s) ]\n" RESET, sgl, _str_sgl(sgl));
  signal(sgl, caught);
}

int default_main(int cnt, ...) {
  curr = def;
  va_list args;
  va_start(args, cnt);
  printf(BOLD
    "\t+-------------------------------+\n"
    "\t|         STARTING TESTS        |\n"
    "\t+-------------------------------+\n" RESET
  );

  for(int i = 0; i < cnt; i++) {
    va_arg(args, void (*)())();
  }

  printf(BOLD
    "\t+-------------------------------+\n"
    "\t|         TESTS FINISHED        |\n"
    "\t+----------------------+--------+\n" RESET
  );
  print_summary(TRUE);
  va_end(args);

  return tests - testok;
}

int old_stdout = -1, old_stderr = -1;

void mute_stdout() {
  if(old_stdout != -1) return;
  old_stdout = dup(1);
  freopen("/dev/null", "w", stdout);
}

void mute_stderr() {
  if(old_stderr != -1) return;
  old_stderr = dup(2);
  freopen("/dev/null", "w", stderr);
}

void unmute_stdout() {
  if(old_stdout == -1) return;
  char buf[32]; // should be plenty of space
  sprintf(buf, "/dev/fd/%d", old_stdout);
  freopen(buf, "w", stdout);
  old_stdout = -1;
}

void unmute_stderr() {
  if(old_stderr == -1) return;
  char buf[32]; // should be plenty of space
  sprintf(buf, "/dev/fd/%d", old_stderr);
  freopen(buf, "w", stderr);
  old_stderr = -1;
}
