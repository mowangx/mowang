1. google core dumper (https://code.google.com/archive/p/google-coredumper/downloads)
	1.1 CLONE_VM未定义，在头文件中增加
		#define _GNU_SOURCE             /* See feature_test_macros(7) */
		#include <sched.h>
	1.2 linux/dirent.h 找不到，改为dirent.h