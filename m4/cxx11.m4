# cxx11.m4 serial 6
dnl Copyright (C) 2012 Reece H. Dunn
dnl
dnl This file is free software; Reece H. Dunn
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl ================================================================
dnl C++11 support
dnl ================================================================

AC_DEFUN([CXX11_CHECK_CXX11_SUPPORT], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 without any flags])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[template <typename T> struct right_angle_brackets {};]
			 [typedef right_angle_brackets<right_angle_brackets<int>> cpp11_check;]],
			[[]])],
		[AC_MSG_RESULT([yes])
		 have_cxx11_native=yes],
		[AC_MSG_RESULT([no])
		 have_cxx11_native=no])
	AC_LANG_POP(C++)

	AC_LANG_PUSH(C++)
	TEMP_CXXFLAGS="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS -std=c++11"
	AC_MSG_CHECKING([if $CXX supports C++11 with the -std=c++11 flag])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[template <typename T> struct right_angle_brackets {};]
			 [typedef right_angle_brackets<right_angle_brackets<int>> cpp11_check;]],
			[[]])],
		[AC_MSG_RESULT([yes])
		 have_cxx11_cxx11=yes],
		[AC_MSG_RESULT([no])
		 have_cxx11_cxx11=no])
	CXXFLAGS="$TEMP_CXXFLAGS"
	AC_LANG_POP(C++)

	AC_LANG_PUSH(C++)
	TEMP_CXXFLAGS="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS -std=gnu++11"
	AC_MSG_CHECKING([if $CXX supports C++11 with the -std=gnu++11 flag])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[template <typename T> struct right_angle_brackets {};]
			 [typedef right_angle_brackets<right_angle_brackets<int>> cpp11_check;]],
			[[]])],
		[AC_MSG_RESULT([yes])
		 have_cxx11_gnuxx11=yes],
		[AC_MSG_RESULT([no])
		 have_cxx11_gnuxx11=no])
	CXXFLAGS="$TEMP_CXXFLAGS"
	AC_LANG_POP(C++)

	AC_LANG_PUSH(C++)
	TEMP_CXXFLAGS="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS -std=c++0x"
	AC_MSG_CHECKING([if $CXX supports C++11 with the -std=c++0x flag])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[template <typename T> struct right_angle_brackets {};]
			 [typedef right_angle_brackets<right_angle_brackets<int>> cpp11_check;]],
			[[]])],
		[AC_MSG_RESULT([yes])
		 have_cxx11_cxx0x=yes],
		[AC_MSG_RESULT([no])
		 have_cxx11_cxx0x=no])
	CXXFLAGS="$TEMP_CXXFLAGS"
	AC_LANG_POP(C++)

	AC_LANG_PUSH(C++)
	TEMP_CXXFLAGS="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS -std=gnu++0x"
	AC_MSG_CHECKING([if $CXX supports C++11 with the -std=gnu++0x flag])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[template <typename T> struct right_angle_brackets {};]
			 [typedef right_angle_brackets<right_angle_brackets<int>> cpp11_check;]],
			[[]])],
		[AC_MSG_RESULT([yes])
		 have_cxx11_gnuxx0x=yes],
		[AC_MSG_RESULT([no])
		 have_cxx11_gnuxx0x=no])
	CXXFLAGS="$TEMP_CXXFLAGS"
	AC_LANG_POP(C++)

	AC_MSG_CHECKING([if $CXX supports C++11])
	if test "$have_cxx11_cxx11" = yes ; then
		AC_MSG_RESULT([-std=c++11])
		CXXFLAGS="$CXXFLAGS -std=c++11"
	elif test "$have_cxx11_gnuxx11" = yes ; then
		AC_MSG_RESULT([-std=gnu++11])
		CXXFLAGS="$CXXFLAGS -std=gnu++11"
	elif test "$have_cxx11_cxx0x" = yes ; then
		AC_MSG_RESULT([-std=c++0x])
		CXXFLAGS="$CXXFLAGS -std=c++0x"
	elif test "$have_cxx11_gnuxx0x" = yes ; then
		AC_MSG_RESULT([-std=gnu++0x])
		CXXFLAGS="$CXXFLAGS -std=gnu++0x"
	elif test "$have_cxx11_native" = yes ; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([C++11 is not supported by $CXX.])
	fi])

dnl ================================================================
dnl C++11 nullptr
dnl ================================================================

AC_DEFUN([CXX11_NULLPTR], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 nullptr])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[]],
			[[const char *ptr = nullptr;]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 nullptr not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 decltype
dnl ================================================================

AC_DEFUN([CXX11_DECLTYPE], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 decltype])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[]],
			[[int a;]
			 [decltype(a) b = a;]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 decltype not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 auto
dnl ================================================================

AC_DEFUN([CXX11_AUTO], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 auto])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[]],
			[[class A {} a;]
			 [auto b = a;]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 auto not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 range-based for loops
dnl ================================================================

AC_DEFUN([CXX11_RANGE_BASED_FOR], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 range-based for loops])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[]],
			[[int a[] = { 1, 2, 3 };]
			 [for (int &b : a) ++b;]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 range-based for loops not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 brace initialization
dnl ================================================================

AC_DEFUN([CXX11_BRACE_INITIALIZATION], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 brace initialization])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <complex>]],
			[[std::complex<float> c { 0.1, 0.2 };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 brace initialization not supported by $CXX.])])
	dnl Check clang bug 12670 [http://llvm.org/bugs/show_bug.cgi?id=12670]
	AC_MSG_CHECKING([if $CXX supports C++11 brace initialization of a class in a struct])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <complex>]],
			[[struct A { std::complex<float> c; };]
			 [A a { { 0.1, 0.2 } };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 brace initialization of a class in a struct not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 scoped enums
dnl ================================================================

AC_DEFUN([CXX11_SCOPED_ENUMS], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 scoped enums])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[enum class E { A, B };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 scoped enums not supported by $CXX.])])
	AC_MSG_CHECKING([if $CXX supports C++11 scoped enums with explicit size])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[enum class E : char { A, B };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 scoped enums with explicit size not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 constexpr
dnl ================================================================

AC_DEFUN([CXX11_CONSTEXPR], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 constexpr])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[constexpr float value = 1.25;]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 constexpr not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 lambda
dnl ================================================================

AC_DEFUN([CXX11_LAMBDA], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 lambda])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[auto f = [](int x) -> int { return x; };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 lambda not supported by $CXX.])])
	AC_LANG_POP(C++)])

AC_DEFUN([CXX11_LAMBDA_CAPTURE], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 lambda captures])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[int val = 0;]
			 [auto f = [&val](int x) { val = x; };]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 lambda captures not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 std::initializer_list
dnl ================================================================

AC_DEFUN([CXX11_HAVE_STD_INITIALIZER_LIST], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::initializer_list])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <vector>]
			 [std::initializer_list<int> il = { 1, 2, 3, 4 };]],
			[[std::vector<int> v(il);]])],
		[AC_MSG_RESULT([yes])
		 AC_DEFINE([HAVE_CXX11_STD_INITIALIZER_LIST], [1], [Define to 1 if std::initializer_list is supported])],
		[AC_MSG_RESULT([no])])
	AC_LANG_POP(C++)])

AC_DEFUN([CXX11_REQUIRE_STD_INITIALIZER_LIST], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::initializer_list])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <vector>]
			 [std::initializer_list<int> il = { 1, 2, 3, 4 };]],
			[[std::vector<int> v(il);]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 std::initializer_list not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 std::shared_ptr
dnl ================================================================

AC_DEFUN([CXX11_HAVE_STD_SHARED_PTR], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::shared_ptr])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <memory>]],
			[[std::shared_ptr<int> p = std::make_shared<int>(5);]])],
		[AC_MSG_RESULT([yes])
		 AC_DEFINE([HAVE_CXX11_STD_SHARED_PTR], [1], [Define to 1 if std::shared_ptr is supported])],
		[AC_MSG_RESULT([no])])
	AC_LANG_POP(C++)])

AC_DEFUN([CXX11_REQUIRE_STD_SHARED_PTR], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::shared_ptr])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <memory>]],
			[[std::shared_ptr<int> p = std::make_shared<int>(5);]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 std::shared_ptr not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 std::begin
dnl ================================================================

AC_DEFUN([CXX11_HAVE_STD_BEGIN], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::begin])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <iterator>]],
			[[int data[] = { 1, 2, 3, 4, 5 };]
			 [int *first = std::begin(data);]])],
		[AC_MSG_RESULT([yes])
		 AC_DEFINE([HAVE_CXX11_STD_BEGIN],[1],[Define to 1 if std::begin is supported.])],
		[AC_MSG_RESULT([no])])
	AC_LANG_POP(C++)])

AC_DEFUN([CXX11_REQUIRE_STD_BEGIN], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::begin])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <iterator>]],
			[[int data[] = { 1, 2, 3, 4, 5 };]
			 [int *first = std::begin(data);]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 std::begin not supported by $CXX.])])
	AC_LANG_POP(C++)])

dnl ================================================================
dnl C++11 std::end
dnl ================================================================

AC_DEFUN([CXX11_HAVE_STD_END], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::end])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <iterator>]],
			[[int data[] = { 1, 2, 3, 4, 5 };]
			 [int *last  = std::end(data);]])],
		[AC_MSG_RESULT([yes])
		 AC_DEFINE([HAVE_CXX11_STD_END],[1],[Define to 1 if std::end is supported.])],
		[AC_MSG_RESULT([no])])
	AC_LANG_POP(C++)])

AC_DEFUN([CXX11_REQUIRE_STD_END], [
	AC_LANG_PUSH(C++)
	AC_MSG_CHECKING([if $CXX supports C++11 std::end])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[#include <iterator>]],
			[[int data[] = { 1, 2, 3, 4, 5 };]
			 [int *last  = std::end(data);]])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])
		 AC_MSG_ERROR([C++11 std::end not supported by $CXX.])])
	AC_LANG_POP(C++)])
