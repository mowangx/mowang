#!bin/sh

echo 'CPP_SRCS += \' > makefile.txt
ls -a -1 *.cpp|awk -F'.' '{ print "\${SRC_DIR}/src/"$1".cpp \\"}' >> makefile.txt
echo "" >> makefile.txt
echo "" >> makefile.txt

echo 'OBJS += \' >> makefile.txt
ls -a -1 *.cpp|awk -F'.' '{ print "\${OBJ_DIR}/src/"$1".o \\"}' >> makefile.txt
echo "" >> makefile.txt
echo "" >> makefile.txt

echo 'CPP_DEPS += \' >> makefile.txt
ls -a -1 *.cpp|awk -F'.' '{ print "\${DEP_DIR}/src/"$1".d \\"}' >> makefile.txt
echo "" >> makefile.txt
echo "" >> makefile.txt

cat makefile.in >> makefile.txt
mv makefile.txt makefile.mk
