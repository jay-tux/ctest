#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>
#include <signal.h>

typedef unsigned short ushort;
typedef unsigned int uint;

#define XSIGNALS \
  X(SIG_TERMINATED, SIGTERM) \
  X(SIG_ABORTED, SIGABRT) \
  X(SIG_FLOATERR, SIGFPE) \
  X(SIG_SEGMFAULT, SIGSEGV) \
  X(SIG_INTERRUPT, SIGINT) \
  X(SIG_INSTRUCT, SIGILL)

typedef enum sig {
#define X(s_new, s_og) s_new = s_og,
XSIGNALS
#undef X

  SIG_CATCHALL   = 0xff
} sig_e;

void start_test(const char *test);
void abort_test();
void end_test();
void checks();
void success();
void catch_sig(sig_e signal);
int default_main(int cnt, ...);

// Following code based on https://bytes.com/topic/c/answers/811133-temporarily-close-stdout
void mute_stdout();
void unmute_stdout();
void mute_stderr();
void unmute_stderr();

#ifndef NO_COLOR
#  define BLUE    "\033[0;36m"
#  define GREEN   "\033[0;32m"
#  define BRED    "\033[0;41m"
#  define RED     "\033[0;31m"
#  define BYELLOW "\033[0;43m"
#  define RESET   "\033[0;0m"
#  define BOLD    "\033[0;1m"
#else
#  define BLUE    ""
#  define GREEN   ""
#  define RED     ""
#  define BRED    ""
#  define BYELLOW ""
#  define RESET   ""
#  define BOLD    ""
#endif

#define ASSERT_BOILER(check, fatal, err, ...) { checks(); if(check) { fprintf(stderr, RED BOLD __FILE__ " [%d]: " err "\n" RESET, __LINE__, __VA_ARGS__); if(fatal) { abort_test(); return; } } else { success(); } }
#define ASSERT_NULL(v1, fatal, err, ...) ASSERT_BOILER((v1) != NULL, fatal, err, __VA_ARGS__)
#define ASSERT_NNULL(v1, fatal, err, ...) ASSERT_BOILER((v1) == NULL, fatal, err, __VA_ARGS__)
#define ASSERT_EQUAL(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) != v2, fatal, err, __VA_ARGS__)
#define ASSERT_NOTEQUAL(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) == v2, fatal, err, __VA_ARGS__)
#define ASSERT_GREATER(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) <= v2, fatal, err, __VA_ARGS__)
#define ASSERT_SMALLER(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) >= v2, fatal, err, __VA_ARGS__)
#define ASSERT_GREATEREQ(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) < v2, fatal, err, __VA_ARGS__)
#define ASSERT_SMALLEREQ(v1, v2, fatal, err, ...) ASSERT_BOILER((v1) > v2, fatal, err, __VA_ARGS__)
#define ASSERT_TRUE(v1, fatal, err, ...) ASSERT_BOILER(!(v1), fatal, err, __VA_ARGS__)
#define ASSERT_FALSE(v1, fatal, err, ...) ASSERT_BOILER((v1), fatal, err, __VA_ARGS__)

typedef ushort bool;
#define FALSE 0
#define TRUE 1

#endif
