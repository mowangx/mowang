1. google core dumper (https://code.google.com/archive/p/google-coredumper/downloads)
	1.1 CLONE_VMδ���壬��ͷ�ļ�������
		#define _GNU_SOURCE             /* See feature_test_macros(7) */
		#include <sched.h>
	1.2 linux/dirent.h �Ҳ�������Ϊdirent.h
	1.3 ./configure && make && make install
	
2. ld -lshare_d --verbose (��ͨ��ld --help)

3. libevent (http://libevent.org/) 
    3. 1 ./configure && make && make install
	
4. tinyxpath(http://sourceforge.net/projects/tinyxpath)
	4.1 copy trunk/doc/makefile

5. boost
    5.1 windows��̬��(https://dl.bintray.com/boostorg/release/1.65.1/binaries/)
	5.2 Դ����(https://www.boost.org/users/history/)
	5.3 https://www.cnblogs.com/songxingzhu/p/7513519.html
	5.4 .\b2 toolset=msvc-14.1 --build-type=complete --with-python python=2.7 -- address-model=64 link=static threading=multi install -j4 --prefix=E:\favourite\boost_1_65_1\boost_install --libdir=E:\favourite\boost_1_65_1\boost_lib --includedir=E:\favourite\boost_1_65_1\boost_include
	
6. openssl(https://github.com/openssl/openssl)
	6.1 cur version is 1.1.0
	6.2 build reference https://www.cnblogs.com/chinalantian/p/5819105.html  &&  https://blog.csdn.net/u013214392/article/details/78621707
	6.3 visual studio tools --> vc ---> ������x64�����������ʾ��
	
7. wss
	7.1 ��ǩ֤�� https://www.cnblogs.com/liqingjht/p/6267563.html  && https://blog.csdn.net/oldmtn/article/details/52208747 && https://segmentfault.com/a/1190000002554673
	7.2 server֤����������client֤��������

8. nginx
	8.1 ���nginx yum��Դ�⣺rpm -Uvh http://nginx.org/packages/centos/7/noarch/RPMS/nginx-release-centos-7-0.el7.ngx.noarch.rpm
	8.2 ����nginx��bind() to x.x.x.x:port failed, ִ��setenforce 0������SELinux ��Ϊpermissiveģʽ
	8.3 wget��post��ʽ��wget --post-data='{"user_name":"mowang","password":"abc123"}' localhost:9010/user/login

9. pip
	9.1 ��װepel��չԴ: yum -y install epel-release
	9.2 ��װpip�� yum -y install python-pip

	
