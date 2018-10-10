#命令别名
RM := rm -rf
CC=g++
	#编译前
	#编译目标
	#编译后
#=============所有的编译目标(请不要随便移动此行的位置)=============
all: start share.lib finish				

WORKSPACE=..
#=============包含(inc/src/lib)目录=============
#share
SHARE_DIR=${WORKSPACE}/../share/
SHARE_INC=${SHARE_DIR}/src/
#libevent目录
EVENT_INC=${SHARE_INC}/libevent/
#mysqlpp目录
#MYSQLPP_INC=${SHARE_INC}/mysqlpp/
MYSQL_INC="/usr/include/mysql/"
#tinyxml
TINYXPATH_INC=${SHARE_INC}/tinyxpath/
#boost
BOOST_INC=${SHARE_INC}/boost/
#core_dumper
CORE_DUMPER_INC=${SHARE_INC}/core_dumper/
#websocketpp
WEBSOCKET_INC=${SHARE_INC}/websocketpp/
#openssl
OPENSSL_INC=${SHARE_INC}/openssl/

#源码目录
SRC_DIR=${WORKSPACE}/

#库目录
THIRD_LIB_DIR=${WORKSPACE}/../lib/
#MYSQLPP_LIB_DIR=${THIRD_LIB_DIR}
MYSQL_LIB_DIR=/usr/local/lib/

#=============编译后(obj/exe/lib)目录=============
#obj目录
OBJ_DIR=obj
DEP_DIR=dep
EXE_DIR=.
DEST_LIB_DIR=${THIRD_LIB_DIR}

#=============编译选项=============
#编译包含的头文件
CC_INC=-I"${SRC_DIR}" -I"${SRC_DIR}/src/" -I${EVENT_INC} -I${MYSQL_INC} -I${MYSQLPP_INC} -I${CORE_DUMPER_INC} -I${WEBSOCKET_INC} -I${OPENSSL_INC}
#编译优化
CC_OPT=-O0
#编译忽略的警告
CC_IGO=-Wno-deprecated -Wno-unused-function -Wno-unused
#编译的标记
CC_FLAG=-g3 -Wall -c -fmessage-length=0 -MMD -MP -fstack-protector-all -std=c++11
#CC_FLAG=-g3 -Wall -Werror -c -fmessage-length=0 -MMD -MP -fstack-protector-all -std=c++11
#编译依赖关系
CC_DEP=-MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
#编译obj选项
CC_OJB=-o "$@" "$<"
#预编译宏定义
CC_DEF=-DLIB_DEBUG -DSERVER_DEBUG

#=============链接选项=============
# 系统库
LK_SYS_LIB=
# 第三方库
LK_DEP_LIB=
# 内部库
LK_USER_LIB=

#=============包含源码目录下的makefile文件=============
-include ${SRC_DIR}/src/makefile.mk

ifneq ($(MAKECMDGOALS),clean)
#避免多次包含
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
endif

#=============编译前=============
start:
	@echo "compiler start!"
	@if [ ! -d $(OBJ_DIR) ]; then\
		echo "create dir ${OBJ_DIR}";\
		mkdir "${OBJ_DIR}";\
	fi
	@if [ ! -d $(OBJ_DIR)/src/ ]; then\
		echo "create dir ${OBJ_DIR}/src";\
		mkdir "${OBJ_DIR}/src";\
	fi

	@if [ ! -d "${DEP_DIR}" ]; then\
		echo "create dir ${DEP_DIR}";\
		mkdir "${DEP_DIR}";\
	fi
	@if [ ! -d "${DEP_DIR}/src/" ]; then\
		echo "create dir ${DEP_DIR}/src/";\
		mkdir "${DEP_DIR}/src/";\
	fi
	@echo " "
	@echo " "

#=============编译后=============
finish: 
	@mv libshare_d.a ${DEST_LIB_DIR}/libshare_d.a
	@echo " "
	@echo " "
	@echo "compiler finish!"

# =============生成的lib目标=============
share.lib: ${OBJS}
	@echo 'Building target: $@'
	@echo 'Invoking: gnu C++ Linker'
	ar rcs libshare_d.a ${OBJS}
	@echo 'Finished building target: $@'
	@echo ' '

#=============清理=============
clean:
	-$(RM) $(OBJ_DIR) $(DEP_DIR)
	-@echo ' '

#=============伪目标=============
.PHONY: all clean dependents
#禁止删除中间目标
.SECONDARY:
