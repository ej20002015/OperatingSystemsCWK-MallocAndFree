#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "memory_management.h"

/*
_malloc(size) returns a valid pointer
_malloc(0) return a null pointer
_malloc(size), _malloc(size0)
*/


/*
determine if _malloc returns a null ptr when it is not meant to.
*/
START_TEST(test_malloc_not_null) {
	void * ptr = _malloc(100);
	ck_assert_msg(ptr!=NULL, "malloc returns null ptr (0)");

	void * ptr1 = _malloc(1);
	ck_assert_msg(ptr1!=NULL, "malloc returns null ptr (1)");

	void * ptr2 = _malloc(4097);
	ck_assert_msg(ptr2!=NULL, "malloc returns null ptr (2)");

} END_TEST

START_TEST(test_malloc_null) {
	void * ptr = _malloc(0);
	ck_assert_msg(ptr==NULL, "malloc returned non-null ptr");
} END_TEST

START_TEST(test_malloc_sbrk) {
	void * before_ptr = sbrk(0);
	char * malloc_ptr = _malloc(100);
	void * after_ptr = sbrk(0);
	ck_assert_msg(before_ptr < after_ptr,"malloc doesn't appear to call sbrk");
} END_TEST

START_TEST(test_malloc_ptr) {
	char * ptr = _malloc(100);
	char * ptr1 = _malloc(100);
	ck_assert_msg(ptr + 100  < ptr1,"Two calls to malloc with insufficient distance between pointers");
} END_TEST

START_TEST(test_malloc_massive) {
	char * before_ptr = sbrk(0);
	char * ptr = _malloc(8190);
	char * after_ptr = sbrk(0);
	ck_assert_msg(after_ptr - before_ptr >= 8190 ,"_mallloc appears not to allocate enough memory");
} END_TEST

START_TEST(test_malloc_too_much) {
	char * before_ptr = sbrk(0);
	char * malloc_ptr = _malloc(100);
	char * after_ptr = sbrk(0);
	ck_assert_msg(after_ptr - before_ptr < 65536 ,"Mallloc appears to well over allocate");
} END_TEST

START_TEST(test_malloc_align){
	char * ptr = _malloc(10);
	char * ptr1 = _malloc(3);
	ck_assert_msg(((long) ptr) % 8 == 0,"_malloc does not align memory");
	ck_assert_msg(((long) ptr1) % 8 == 0,"_malloc does not align memory");
} END_TEST

/*
_free tests
*/
START_TEST(test_alloc) {
	void * ptr = _malloc(100);
	_free(ptr);
	ck_assert(1);
} END_TEST

START_TEST(test_alloc_null_ptr) {
	void * ptr = _malloc(0);
	_free(ptr);
	ck_assert(1);
} END_TEST

START_TEST(test_alloc_give_back) {
	void * ptr = _malloc(8190);
	char * before_ptr = sbrk(0);
	_free(ptr);
	char * after_ptr = sbrk(0);
	ck_assert_msg(after_ptr < before_ptr ,"_free does not appear to return memory");
} END_TEST


START_TEST(test_reuse_space) {
	char * ptr_0 = _malloc(1000);
	char * ptr_1 = _malloc(1000);
	char * ptr_2 = _malloc(1000);
	_free(ptr_0);
	char * ptr_3 = _malloc(100);
	ck_assert_msg(ptr_3 < ptr_1 ,"malloc does not appear to reuse memory");
	ck_assert_msg(ptr_3 >= ptr_0 ,"malloc does not appear to reuse memory");
} END_TEST


START_TEST(test_merge_space) {
	char * ptr_0 = _malloc(1000);
	char * ptr_1 = _malloc(1000);
	char * ptr_2 = _malloc(1000);
	_free(ptr_0);
	_free(ptr_1);
	char * ptr_3 = _malloc(1500);
	ck_assert_msg(ptr_3 < ptr_2 ,"free appears not to merge \"blocks\"");
	ck_assert_msg(ptr_3 >= ptr_0 ,"free appears not to merge \"blocks\"");
	ck_assert_msg(ptr_3 <= ptr_1 ,"free appears not to merge \"blocks\"");
} END_TEST

START_TEST(test_merge_space_1) {
	char * ptr_0 = _malloc(10);
	char * ptr_1 = _malloc(10);
	char * ptr_2 = _malloc(10);
	char * before_ptr = sbrk(0);
	_free(ptr_0);
	_free(ptr_1);
	_free(ptr_2);
	char * ptr_3 = _malloc(27);
	char * after_ptr = sbrk(0);
	ck_assert_msg(ptr_1 < after_ptr ,"free appears not to merge \"blocks\"");
} END_TEST

Suite *memory_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Memory management tests");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_malloc_not_null);
	tcase_add_test(tc_core, test_malloc_null);
	tcase_add_test(tc_core, test_malloc_ptr);
	tcase_add_test(tc_core, test_malloc_align);
	tcase_add_test(tc_core, test_malloc_sbrk);
	tcase_add_test(tc_core, test_malloc_massive);
	tcase_add_test(tc_core, test_malloc_too_much);



	tcase_add_test(tc_core, test_alloc);
	tcase_add_test(tc_core, test_alloc_null_ptr);
	tcase_add_test(tc_core, test_alloc_give_back);

	tcase_add_test(tc_core, test_reuse_space);
	tcase_add_test(tc_core, test_merge_space);
	tcase_add_test(tc_core, test_merge_space_1);


  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int no_failed = 0;
  Suite *s;
  SRunner *runner;

  s = memory_suite();
  runner = srunner_create(s);
	srunner_set_log(runner, "test.log");
	srunner_set_fork_status(runner, CK_FORK);

  srunner_run_all(runner, CK_NORMAL);


  no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
return 0;
    //return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
