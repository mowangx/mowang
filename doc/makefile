#命令别名
RM := rm -rf
CC=g++
	#编译前
	#编译目标
	#编译后
#=============所有的编译目标(请不要随便移动此行的位置)=============
all: start src tinyxpath.lib finish				

#=============包含(inc/src/lib)目录=============

DEST_LIB_DIR=/home/mowang/game/server/lib/

#=============编译选项=============
#编译包含的头文件
CC_INC=
#编译优化
CC_OPT=-O0
#编译忽略的警告
CC_IGO=-Wno-deprecated
#编译的标记
CC_FLAG=-g3 -Wall -c -fmessage-length=0 -MMD -MP
#编译依赖关系
CC_DEP=-MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
#编译obj选项
CC_OJB=-o "$@" "$<"
#预编译宏定义
CC_DEF=-DLIB_DEBUG

#=============链接选项=============
# 系统库
LK_SYS_LIB=
# 第三方库
LK_DEP_LIB=
# 内部库
LK_USER_LIB=

CPP_SRCS += \
action_store.cpp	\
htmlutil.cpp	\
lex_util.cpp	\
main.cpp	\
node_set.cpp	\
tinystr.cpp	\
tinyxml.cpp	\
tinyxmlerror.cpp	\
tinyxmlparser.cpp	\
tokenlist.cpp	\
xml_util.cpp	\
xpath_expression.cpp	\
xpath_processor.cpp	\
xpath_stack.cpp	\
xpath_static.cpp	\
xpath_stream.cpp	\
xpath_syntax.cpp

OBJS += \
action_store.o	\
htmlutil.o	\
lex_util.o	\
main.o	\
node_set.o	\
tinystr.o	\
tinyxml.o	\
tinyxmlerror.o	\
tinyxmlparser.o	\
tokenlist.o	\
xml_util.o	\
xpath_expression.o	\
xpath_processor.o	\
xpath_stack.o	\
xpath_static.o	\
xpath_stream.o	\
xpath_syntax.o

CPP_DEPS += \
action_store.d	\
htmlutil.d	\
lex_util.d	\
main.d	\
node_set.d	\
tinystr.d	\
tinyxml.d	\
tinyxmlerror.d	\
tinyxmlparser.d	\
tokenlist.d	\
xml_util.d	\
xpath_expression.d	\
xpath_processor.d	\
xpath_stack.d	\
xpath_static.d	\
xpath_stream.d	\
xpath_syntax.d


src: ${OBJS}

# Each subdirectory must supply rules for building sources it contributes
${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	@echo 'Building file: $<'
	${CC} ${CC_DEF} ${CC_INC} ${CC_OPT} ${CC_FLAG} -MF"$(@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d)" \
		-MT"$(@:${OBJ_DIR}/%.o=${DEP_DIR}/%.d)" -o "$@" "$<" ${CC_IGO}
	@echo 'Finished building: $<'
	@echo ' '

#=============编译前=============
start:
	@echo "compiler start!"
	@echo " "
	@echo " "

#=============编译后=============
finish: 
	@cp libtinyxpath.a ${DEST_LIB_DIR}
	@echo " "
	@echo " "
	@echo "compiler finish!"

# =============生成的lib目标=============
tinyxpath.lib: ${OBJS}
	@echo 'Building target: $@'
	@echo 'Invoking: gnu C++ Linker'
	ar rcs libtinyxpath.a ${OBJS}
	@echo 'Finished building target: $@'
	@echo ' '

#=============清理=============
clean:
	-$(RM) $(OBJ_DIR) $(DEP_DIR)
	-${RM} *.o
	-@echo ' '

#=============伪目标=============
.PHONY: all clean dependents
#禁止删除中间目标
.SECONDARY: