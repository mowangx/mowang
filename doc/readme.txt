1. google core dumper (https://code.google.com/archive/p/google-coredumper/downloads)
	1.1 CLONE_VM未定义，在头文件中增加
		#define _GNU_SOURCE             /* See feature_test_macros(7) */
		#include <sched.h>
	1.2 linux/dirent.h 找不到，改为dirent.h
	1.3 ./configure && make && make install
	
2. ld -lshare_d --verbose (可通过ld --help)

3. libevent (http://libevent.org/) 
    3. 1 ./configure && make && make install
	
4. tinyxpath(http://sourceforge.net/projects/tinyxpath)
	4.1 copy trunk/doc/makefile

5. boost
    5.1 windows动态库(https://dl.bintray.com/boostorg/release/1.65.1/binaries/)
	5.2 源代码(https://www.boost.org/users/history/)
