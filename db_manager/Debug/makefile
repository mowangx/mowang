#命令别名
RM := rm -rf
CC=g++
#SERVER_VERSION=`svn info ../../|grep ^版本: |awk  '{print $2}'`
#BUILD_DATE=`date "+%y-%m-%d %H:%M"`

#编译前
#编译目标
#编译后
#=============所有的编译目标(请不要随便移动此行的位置)=============
all: start db_manager db_managerexe finish				

WORKSPACE=..
#=============包含(inc/src/lib)目录=============
#share
SHARE_DIR=${WORKSPACE}/../share/
SHARE_INC=${SHARE_DIR}/src/
#libevent目录
EVENT_INC=${SHARE_INC}/libevent/
#mysql目录
MYSQL_INC=${SHARE_INC}/mysql/
#MYSQL_INC="/usr/include/mysql/"
#tinyxml
TINYXPATH_INC=${SHARE_INC}/tinyxpath/
#boost
BOOST_INC=${SHARE_INC}/boost/

#源码目录
SRC_DIR=${WORKSPACE}/

#库目录
THIRD_LIB_DIR=${WORKSPACE}/../lib/
#MYSQLPP_LIB_DIR=${THIRD_LIB_DIR}
MYSQL_LIB_DIR=${THIRD_LIB_DIR}/mysql_lib/

#=============编译后(obj/exe/lib)目录=============
#obj目录
OBJ_DIR=obj
DEP_DIR=dep
EXE_DIR=${WORKSPACE}/../exe/db_manager/
DEST_LIB_DIR=
EXE_NAME=db_manager
#THIRD_LIB=${MYSQLPP_LIB_DIR}/*.so*

#=============编译选项=============
#编译包含的头文件
CC_INC=-I"${SRC_DIR}" -I"${SRC_DIR}/src/" -I${EVENT_INC} -I${MYSQL_INC}	-I${TINYXPATH_INC} -I${SHARE_INC}
#编译优化
CC_OPT=-O
#编译忽略的警告
CC_IGO=-Wno-deprecated -Wno-unused-function
#编译的标记
CC_FLAG=-g3 -Wall -c -fmessage-length=0 -MMD -MP -fstack-protector-all -std=c++11
#编译依赖关系
CC_DEP=-MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
#编译obj选项
CC_OJB=-o "$@" "$<"
#预编译宏定义
#CC_DEF=-DLIB_DEBUG -DSERVER_DEBUG -DSERVER_VERSION=$(SERVER_VERSION) -DBUILD_DATE=$(BUILD_DATE)
CC_DEF=-DLIB_DEBUG -DSERVER_DEBUG

#=============链接选项=============
# 系统库
LK_SYS_LIB=-Wl,-Bdynamic,-lrt -Wl,-Bdynamic,-lpthread -Wl,-Bdynamic,-ldl -Wl,-Bdynamic,-lcrypto -Wl,-Bdynamic,-lssl -Wl,-Bdynamic,-lmysqlcppconn
# 第三方库
LK_DEP_LIB=-L. -L${THIRD_LIB_DIR} -Wl,-Bstatic,-ltinyxpath -Wl,-Bstatic,-lshare_d -Wl,-Bstatic,-levent -Wl,-Bstatic,-lcoredumper

# 内部库
LK_USER_LIB=
	
# Each subdirectory must supply rules for building sources it contributes
${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	@echo 'Building file: $<'
	${CC} ${CC_DEF} ${CC_INC} ${CC_OPT} ${CC_FLAG} -MF"$(@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d)" \
		-MT"$(@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d)" -o "$@" "$<" ${CC_IGO}
	@echo 'Finished building: $<'
	@echo ' '

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
	@if [ ! -d $(OBJ_DIR)/src/ ]; then\
		echo "create dir ${OBJ_DIR}/src/";\
		mkdir -p "${OBJ_DIR}/src/";\
	fi

	@if [ ! -d "${DEP_DIR}/src/" ]; then\
		echo "create dir ${DEP_DIR}/src/";\
		mkdir -p "${DEP_DIR}/src/";\
	fi
	
	@if [ ! -d "${EXE_DIR}" ]; then\
		echo "create dir ${EXE_DIR}";\
		mkdir -p "${EXE_DIR}";\
	fi

	@echo " "
	@echo " "

#=============编译后=============
finish: 
	@cp ${EXE_NAME} ${EXE_DIR}
#	@cp ${THIRD_LIB} ${EXE_DIR}
	@echo " "
	@echo " "
	@echo "compiler finish!"

# =============生成的lib目标=============
db_manager: ${OBJS}

db_managerexe:
	@echo 'Building target: $@'
	@echo 'Invoking: gnu C++ Linker'
	${CC} -g3 -o ${EXE_NAME} ${OBJS} ${LK_DEP_LIB} ${LK_SYS_LIB}
	@echo 'Finished building target: $@'
	@echo ' '

#=============清理=============
clean:
	-$(RM) $(OBJ_DIR) $(DEP_DIR)
	-${RM} ${EXE_NAME}
	-@echo ' '

#=============伪目标=============
.PHONY: all clean dependents
#禁止删除中间目标
.SECONDARY:
