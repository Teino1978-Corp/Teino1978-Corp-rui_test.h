//
//  rui_test.h
//  Quanta
//
//  Created by Justin Bowes on 2014-11-06.
//  Copyright (c) 2014 Informi Software Inc. All rights reserved.
//

#ifndef rui_test__B2BD1D7_F65A_4026_B67F_437E3C640A18_h
#define rui_test__B2BD1D7_F65A_4026_B67F_437E3C640A18_h

#ifdef RUI_TEST_INSTRUCTIONS
/*
 # RUI Test
 -------------------------------------------------------------------------------------------

 ## How do I...

 --
 #include "rui_test.h"

 rt_test(sanity) {
	rt_must(true);
	rt_must_not(false);
 }
 --

 That's it.

 ## No, how do I really...

 --
 #include "rui_test.h"
 #include "myobject.h"

 #define RT_BREAK_ON_FAILURE

 static myobject_t *obj;
 static const char *test_str = "gom jabbar"
 static size_t test_data_size;

 rt_suite_begin() {
	obj = myobject_new();
	test_data_size = sizeof(test_str);
 }
 rt_suite_end() {
	myobject_free(obj);
 }
 rt_test_begin() {
	myobject_add(test_str);
 }
 rt_test_end() {
	myobject_clear(obj);
 }

 rt_test(strings) {
	rt_must_be_equal(myobject_size(obj), test_data_size + 1);
	rt_strings_must_be_same(myobject_tostr(obj), test_str);
	rt_numbers_must_be_within(myobject_size(obj) * 0.1f, test_data_size * 0.1f, 0.001f)
 }

 rt_test(floats) {
	rt_include(strings);
	rt_must_be_null(myobject_tofloat(obj));
 }

 #include <math.h>
 #include <complex.h>
 rt_test(sanity) {
	float complex euler_val = cpowf(M_E, I * M_PI);
	bool eulers_identity = (creal(euler_val) == -1.f && cimag(euler_val) == 0.f);
	rt_assert(eulers_identity,
		  "Universe error", "e^(i*π) == -1 should be true, but was [%f, %f]. Check local value of true.",
		  creal(euler_val), cimag(euler_val));
 }

 */
#endif

#define RUI_TEST

#define rt_must(v) rt_assert(v, "must")
#define rt_must_not_be_null(v) rt_assert((v) != NULL, "must not be null")
#define rt_must_be_null(v) rt_assert((v) == NULL, "must not be null")
#define rt_must_not(v) rt_assert(!(v), "must not")
#define rt_must_be_equal(a, b) rt_must((a) == (b), "must be equal")
#define rt_must_not_be_equal(a, b) rt_assert((a) != (b), "must not be equal")
#define rt_strings_must_be_same(a, b) rt_assert_fmt(strcmp(a, b) == 0, "strings must be same", "%s == %s", a, b)
#define rt_numbers_must_be_within(a, b, within)                                                                                       \
	rt_assert(fabs(a - b) < within, "numbers must be within", "%f ~= %f [+/- %f]", (double)a, (double)b, (double)within)


#define rt_test_begin()                                                                                                               \
	static void rt_test_setup_(void);                                                                                             \
	RT_STATIC_INIT(rt_test_setup_init) {                                                                                          \
		rt_test_setup_fn = rt_test_setup_;                                                                                    \
	}                                                                                                                             \
	static void rt_test_setup_(void)

#define rt_test_end()                                                                                                                 \
	static void rt_test_teardown_(void);                                                                                          \
	RT_STATIC_INIT(rt_test_teardown_init) {                                                                                       \
		rt_test_teardown_fn = rt_test_teardown_;                                                                              \
	}                                                                                                                             \
	static void rt_test_teardown_(void)

#define rt_suite_begin()                                                                                                              \
	static void rt_suite_setup_(void);                                                                                            \
	RT_STATIC_INIT(rt_suite_setup_init) {                                                                                         \
		rt_suite_setup_fn = rt_suite_setup_;                                                                                  \
	}                                                                                                                             \
	static void rt_suite_setup_(void)

#define rt_suite_end()                                                                                                                \
	static void rt_suite_teardown_(void);                                                                                         \
	RT_STATIC_INIT(rt_suite_teardown_init) {                                                                                      \
		rt_suite_teardown_fn = rt_suite_teardown_;                                                                            \
	}                                                                                                                             \
	static void rt_suite_teardown_(void)


#define rt_test(named)                                                                                                                \
	static void test_##named(void);                                                                                               \
	RT_STATIC_INIT(add_test_##named) {                                                                                            \
		rt_tests[rt_test_count] = test_##named;                                                                               \
		rt_test_names[rt_test_count] = "test_" #named;                                                                        \
		++rt_test_count;                                                                                                      \
	}                                                                                                                             \
	static void test_##named(void)

#define rt_include(included) test_##included()

#define XXrt_test(named) static void disabled_test_##named(void)

#define RT_DIVMAJ "================================================================================================================\n"

#if _MSC_VER
//	CRT static initializer
#define RTCCALL __cdecl
#pragma section(".CRT$XCU", read)
#define RT_STATIC_INIT(f)                                                                                                             \
	static void RTCCALL f(void);                                                                                                  \
	__declspec(allocate(".CRT$XCU")) void(RTCCALL * f##_)(void) = f;                                                              \
	static void RTCCALL f(void)

#elif __GNUC__ || __clang__
//	GCC + Clang static initializer
#define RT_STATIC_INIT(f)                                                                                                             \
	static void f(void) __attribute__((constructor));                                                                             \
	static void f(void)
#endif


#define rt_print(f, t, x)                                                                                                             \
	fprintf(f, "[%30s %s%40s:%-5d] %s\n", t, strlen(__FILE__) > 40 ? "..." : "",                                                  \
		__FILE__ + (strlen(__FILE__) > 40 ? strlen(__FILE__) - 40 : 0), __LINE__, x)

#define rt_info(x) rt_print(stdout, rt_test_current_name, x)
#define rt_squawk(x) rt_print(stderr, rt_test_current_name, x)

#ifdef RT_BREAK_ON_FAILURE
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define rt_breakpoint_or_not() DebugBreak()
#else
#include <signal.h>
#define rt_breakpoint_or_not() raise(SIGTRAP)
#endif
#else
#define rt_breakpoint_or_not()
#endif

#define rt_fail(why)                                                                                                                  \
	do {                                                                                                                          \
		char rt_failbuf[1024];                                                                                                \
		snprintf(rt_failbuf, 1024, "fail < %s", why);                                                                         \
		rt_failures[rt_failed++] = rt_test_index;                                                                             \
		rt_set_result = true;                                                                                                 \
		rt_squawk(rt_failbuf);                                                                                                \
		rt_breakpoint_or_not();                                                                                               \
	} while (0)
#define rt_pass                                                                                                                       \
	do {                                                                                                                          \
		++rt_passed;                                                                                                          \
		rt_set_result = true;                                                                                                 \
		return;                                                                                                               \
	} while (0)

#define rt_assert(v, desc, ...)                                                                                                       \
	if (!(v)) rt_fail(desc " < " #v)
#define rt_assert_fmt(v, desc, fmt, ...)                                                                                              \
	if (!(v)) {                                                                                                                   \
		char assert_buf[1024];                                                                                                \
		snprintf(assert_buf, 1024, "%s < " fmt " (%s)", desc, __VA_ARGS__, #v);                                               \
		rt_fail(assert_buf);                                                                                                  \
	}


#define RT_DIVSEP "----------------------------------------------------------------------------------------------------------------\n"
#define RT_SUMMARY_FMT "\t%10s:	%10zu %s\n"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define rt_insert                                                                                                                     \
	typedef void (*rt_test_fn)(void);                                                                                             \
	static rt_test_fn rt_tests[1024];                                                                                             \
	static const char *rt_test_names[1024];                                                                                       \
                                                                                                                                      \
	static rt_test_fn rt_suite_setup_fn;                                                                                          \
	static rt_test_fn rt_test_setup_fn;                                                                                           \
	static rt_test_fn rt_test_teardown_fn;                                                                                        \
	static rt_test_fn rt_suite_teardown_fn;                                                                                       \
                                                                                                                                      \
	static size_t rt_test_index;                                                                                                  \
	static const char *rt_test_current_name;                                                                                      \
	static rt_test_fn rt_test_current;                                                                                            \
                                                                                                                                      \
	static size_t rt_test_count;                                                                                                  \
	static bool rt_set_result;                                                                                                    \
	static size_t rt_failed, rt_passed;                                                                                           \
	static size_t rt_failures[1024];                                                                                              \
                                                                                                                                      \
	int main(int argc, const char *argv[]) {                                                                                      \
		if (rt_suite_setup_fn) rt_suite_setup_fn();                                                                           \
		for (rt_test_index = 0; rt_test_index < rt_test_count; ++rt_test_index) {                                             \
			if (rt_test_setup_fn) rt_test_setup_fn();                                                                     \
			rt_test_current = rt_tests[rt_test_index];                                                                    \
			rt_test_current_name = rt_test_names[rt_test_index];                                                          \
			rt_set_result = false;                                                                                        \
			rt_info("starting");                                                                                          \
			rt_test_current();                                                                                            \
			if (!rt_set_result) ++rt_passed;                                                                              \
			rt_failed > 0 && rt_failures[rt_failed - 1] == rt_test_index ? rt_squawk("failed") : rt_info("passed");       \
			if (rt_test_teardown_fn) rt_test_teardown_fn();                                                               \
		}                                                                                                                     \
		if (rt_suite_teardown_fn) rt_suite_teardown_fn();                                                                     \
		FILE *of = rt_failed ? stderr : stdout;                                                                               \
		fprintf(of, RT_DIVSEP);                                                                                               \
		fprintf(of, "Summary\n");                                                                                             \
		fprintf(of, RT_SUMMARY_FMT, "Run", rt_test_count, "=");                                                               \
		fprintf(of, RT_SUMMARY_FMT, "Passed", rt_passed, "+");                                                                \
		fprintf(of, RT_SUMMARY_FMT, "Failed", rt_failed, rt_failed ? "!" : "");                                               \
		if (rt_failed) {                                                                                                      \
			fprintf(of, "\nFailing tests:\n");                                                                            \
			for (size_t i = 0; i < rt_failed; ++i) {                                                                      \
				fprintf(of, "\t%s\n", rt_test_names[rt_failures[i]]);                                                 \
			}                                                                                                             \
		}                                                                                                                     \
		fprintf(of, RT_DIVMAJ);                                                                                               \
	}


#ifndef RT_NO_MAIN
rt_insert
#endif

#endif
