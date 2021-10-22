# CTEST
*Simple unit testing framework for C*

## Table of Contents
 - [Table of Contents](./README.md#table-of-contents)
 - [Features](./README.md#features)
 - [How to Use](./README.md#how-to-use)
   - [Getting Started](./README.md#getting-started)
   - [Tests](./README.md#tests)
   - [Main Function](./README.md#main-function)
   - [Assertions](./README.md#assertions)
   - [Catching Signals](./README.md#catching-signals)
   - [Copy-paste Example](./README.md#copy-paste-example)
 - [Reference](./README.md#reference)
   - [Types](./README.md#types)
   - [Macros](./README.md#macros)
   - [Functions](./README.md#functions)

## Features
 - Easy to use
 - Small: one header, one source file
 - Fancy TUI output
 - Catches segfaults
 - Compile time modifiable: `#define NO_COLOR` prior to inclusion to disable color
 - ... and many more

## How to Use
### Getting Started
All libraries `ctest` uses should be in your system already available (`stdio.h`, `signal.h`, `stdarg.h`, `stdlib.h`, `unistd.h`; all of which are part of the C standard library).  
After downloading/cloning, just put the header and source file in your testing directory. Don't forget to compile `framework.c` as well!

### Tests
After including `framework.h` in your test file, you're ready to go.

Writing a test is easy: just add a new `void` function without arguments. In order to have clean tracking/printing, you should call `start_test("...");` where `...` should be the name of your test. This sets everything up.

Next, you would write setup or initialization code, followed by the assertions. Each of the assertions is available as a macro (see [Assertions](./README.md#assertions)).

Then, add the cleanup code (if needed). Finally, you should call `end_test();` (no arguments indeed) for the pretty-printing of details.

That's it. You have a test.

### Main Function
`ctest` comes with a prepared main function, but you still have to call it yourself. Your own main function will probably look like one of the following examples:

```c
int main() {
  return default_main(n, ...);
}
```

```c
int main() {
  catch_sig(SIG);
  return default_main(n, ...);
}
```

Here, the `n` should be replaced by the amount of test functions you have, and the `...` should be replaced by their names (as function pointers: `void (*)()`).  
If you require signal handling, you can use the `catch_sig(SIG);` function before calling `default_main` to enable the handler; see [Catching Signals](./README.md#catching-signals) for more details.

### Assertions
The library contains multiple assertion macros, for ease of testing. Each of them follows the same general scheme:
```c
ASSERT_<check>(variable(s), fatal, message, ...)
```
Most checks take two variables (or constants) as arguments (some take one). The `fatal` parameter should be the value `TRUE` if a failed assertion should result in the abortion of the test, otherwise `FALSE`. Lastly, the `message` and `...` parameters are `printf`-like, as in, the `message` is the format string and the `...` contains the values to print.

The existing one-variable assertions are (with the signature `ASSERT_<check>(v1, fatal, err, ...)`):
 - `ASSERT_NULL`: fails if the given argument is not equal to `NULL`
 - `ASSERT_NNULL`: fails if the given argument is equal to `NULL`
 - `ASSERT_TRUE`: fails if the given argument is not true
 - `ASSERT_FALSE`: fails if the given argument is not false

The two-variable assertions all correspond to existing operators (with the signature `ASSERT_<check>(v1, v2, fatal, err, ...)`):

 Macro | Corresponding operator
 --- | ---
 `ASSERT_EQUAL` | `==`
 `ASSERT_NOTEQUAL` | `!=`
 `ASSERT_GREATER` | `>`
 `ASSERT_SMALLER` | `<`
 `ASSERT_GREATEREQ` | `>=`
 `ASSERT_SMALLEREQ` | `<=`

**LIMITATION:** due to the macros being used as they are, the `...` argument should never be omitted. If you really don't want to print any arguments, you can always append `"%s"` to the format string and pass `""` as `...` argument.

### Catching Signals
The library is able to catch the following signals (and perform a more "graceful" abortion):
 - `SIGTERM`: the testing was terminated
 - `SIGABRT`: the user aborted the test
 - `SIGFPE`: a floating point error occurred
 - `SIGSEGV`: a segmentation fault occurred
 - `SIGINT`: the user interrupted the test
 - `SIGILL`: the runtime had to execute and illegal instruction

Calling `catch_sig(sig_e signal)` causes `ctest` to catch the given signal (or all signals if `SIG_CATCHALL` is passed). When these signals are caught, the framework will behave as if `abort_test()` has been called, then prints the signal and exits with a non-zero code.

### Copy-paste Example
```c
#include "framework.h"

void some_test() {
  start_test("TEST_NAME");

  // setup here
  // assertions here
  // cleanup here

  end_test();
}

int main() {
  catch_sig(SIG_CATCHALL);           // catch all signals, like segfault, ...
  return default_main(1, some_test); // run 1 test: some_test
}
```

## Reference
*The reference only contains macros and functions meant to be used from outside. Other macros and functions are in the `framework.h` file, but they are requirements for the (public) macros.*
### Types
**ushort**: type alias for `unsigned short`
**bool**: type alias for `unsigned short`
**uint**: type alias for `unsigned int`
**enum sig** enum type with signals:
 - `SIG_TERMINATED`: corresponds to `SIGTERM`
 - `SIG_ABORTED`: corresponds to `SIGABRT`
 - `SIG_FLOATERR`: corresponds to `SIGFPE`
 - `SIG_SEGMFAULT`: corresponds to `SIGSEGV`
 - `SIG_INTERRUPT`: corresponds to `SIGINT`
 - `SIG_INSTRUCT`: corresponds to `SIGILL`
 - `SIG_CATCHALL`: ease-of-use value; corresponds to all of the above
**sig_e**: type alias for `enum sig`

### Macros
- *Function-like macros*
 - `ASSERT_NULL(v1, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 != NULL`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_NNULL(v1, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 == NULL`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_EQUAL(v1, v2, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 != v2`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_GREATER(v1, v2, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 <= v2`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_SMALLER(v1, v2, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 >= v2`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_GREATEREQ(v1, v2, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 < v2`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_SMALLEREQ(v1, v2, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1 > v2`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_TRUE(v1, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `!v1`. If `fatal == TRUE`, aborts the test.
 - `ASSERT_FALSE(v1, fatal, err, ...)`  
   Prints `err` as format string, with substitution in `...` if `v1`. If `fatal == TRUE`, aborts the test.

- *Value Macros:*
 - `#define FALSE 0`
 - `#define TRUE 1`

### Functions
 - `void start_test(const char *test)`  
   Starts a new test with the given name. Does all initialization needed.
 - `void abort_test()`  
   Signals the current test as failed and lost beyond repair. Quits the environment with code `-1`.
 - `void end_test()`  
   Signals the current test as finished. Does the required cleanup and pretty-prints results.
 - `void catch_sig(sig_e signal)`  
   Enables the signal catcher. Requires only one invocation per signal per run. Pass `SIG_CATCHALL` to enable all signal catchers at once.
 - `void default_main(int cnt, ...)`  
   Meant to be used as a default main function. Calls `cnt` functions passed in `...`. Each argument in `...` should be of the type `void (*)()` (pointer to a function with signature `void f();`). Returns with a suggested exit code (the amount of failed tests).
 - `void mute_stdout()`  
   Mutes the standard output stream (redirects to `/dev/null`).
 - `void mute_stderr()`  
   Mutes the standard error stream (redirects to `/dev/null`).
 - `void unmute_stdout()`  
   Unmutes the standard output stream (ends the redirection).
 - `void unmute_stderr()`  
   Unmutes the standard error stream (ends the redirection).
