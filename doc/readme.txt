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
	5.3 https://www.cnblogs.com/songxingzhu/p/7513519.html && https://blog.csdn.net/this_capslock/article/details/47170313
	5.4 .\b2 toolset=msvc-14.1 --build-type=complete --with-python python=2.7 -- address-model=64 link=static threading=multi install -j4 --prefix=E:\favourite\boost_1_65_1\boost_install --libdir=E:\favourite\boost_1_65_1\boost_lib --includedir=E:\favourite\boost_1_65_1\boost_include
	
6. openssl(https://github.com/openssl/openssl)
	6.1 cur version is 1.1.0
	6.2 build reference https://www.cnblogs.com/chinalantian/p/5819105.html  &&  https://blog.csdn.net/u013214392/article/details/78621707
	6.3 visual studio tools --> vc ---> 适用于x64本机命令工具提示符
	
7. wss
	7.1 自签证书 https://www.cnblogs.com/liqingjht/p/6267563.html  && https://blog.csdn.net/oldmtn/article/details/52208747 && https://segmentfault.com/a/1190000002554673
	7.2 server证书有域名，client证书无域名

8. nginx
	8.1 添加nginx yum资源库：rpm -Uvh http://nginx.org/packages/centos/7/noarch/RPMS/nginx-release-centos-7-0.el7.ngx.noarch.rpm
	8.2 启动nginx报bind() to x.x.x.x:port failed, 执行setenforce 0，设置SELinux 成为permissive模式
	8.3 wget的post方式：wget --post-data='{"user_name":"mowang","password":"abc123"}' localhost:9010/user/login

9. pip
	9.1 安装epel扩展源: yum -y install epel-release
	9.2 安装pip： yum -y install python-pip
	
10. etcd
	10.1 api: https://github.com/etcd-io/etcd/blob/master/Documentation/v2/api.md
	10.2 使用共享内存启动： ./etcd --data-dir ./run/shm/ --listen-client-urls http://0.0.0.0:3000 --advertise-client-urls http://0.0.0.0:3000

11. mysql connector:
	11.1 下载源码（1.1.13）： https://dev.mysql.com/downloads/connector/cpp/
	11.2 缺省mysql.h： yum install mysql-devel
	12.3 缺省libmysqlclient.so： yum install mysql-server && mysql install mysql && ln -s /usr/lib64/mysql/libmysqlclient.so.18 /usr/lib/libmysqlclient.so
	12.4 编译:cmake. && make clean && make && make install
	12.5 cp /usr/include/mysql.*.h share/src/mysql && cp -a /usr/include/cppconn share/src/mysql/

	
