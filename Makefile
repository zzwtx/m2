# 使用C++编译器和标志
CXX = g++
CXXFLAGS = -g -Wall -O2 -std=c++11
CPPFLAGS = -DHAVE_KALLOC
INCLUDES = 

# 更新OBJS，添加weighted_m.o
OBJS = kthread.o kalloc.o misc.o bseq.o sketch.o sdust.o options.o index.o \
       lchain.o align.o hit.o seed.o map.o format.o pe.o esterr.o splitidx.o \
       ksw2_ll_sse.o weighted_m.o

PROG = minimap2
PROG_EXTRA = sdust minimap2-lite
LIBS = -lm -lz -lpthread

# 架构相关配置
ifneq ($(aarch64),)
	arm_neon=1
endif

ifeq ($(arm_neon),)
ifeq ($(sse2only),)
	OBJS += ksw2_extz2_sse41.o ksw2_extd2_sse41.o ksw2_exts2_sse41.o ksw2_extz2_sse2.o ksw2_extd2_sse2.o ksw2_exts2_sse2.o ksw2_dispatch.o
else
	OBJS += ksw2_extz2_sse.o ksw2_extd2_sse.o ksw2_exts2_sse.o
endif
else
	OBJS += ksw2_extz2_neon.o ksw2_extd2_neon.o ksw2_exts2_neon.o
	INCLUDES += -Isse2neon
ifeq ($(aarch64),)
	CXXFLAGS += -D_FILE_OFFSET_BITS=64 -mfpu=neon -fsigned-char
else
	CXXFLAGS += -D_FILE_OFFSET_BITS=64 -fsigned-char
endif
endif

# Sanitizer配置
ifneq ($(asan),)
	CXXFLAGS += -fsanitize=address
	LIBS += -fsanitize=address -ldl
endif

ifneq ($(tsan),)
	CXXFLAGS += -fsanitize=thread
	LIBS += -fsanitize=thread -ldl
endif

# 隐式规则：编译.cpp文件
.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

.PHONY: all extra clean depend

all: $(PROG)

extra: all $(PROG_EXTRA)

# 主目标依赖.cpp文件
minimap2: main.cpp libminimap2.a
	$(CXX) $(CXXFLAGS) main.cpp -o $@ -L. -lminimap2 $(LIBS)

minimap2-lite: example.cpp libminimap2.a
	$(CXX) $(CXXFLAGS) example.cpp -o $@ -L. -lminimap2 $(LIBS)

libminimap2.a: $(OBJS)
	$(AR) -csru $@ $(OBJS)

# 显式声明weighted_m.cpp的编译规则
weighted_m.o: weighted_m.cpp weighted_m.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

# SDust工具编译
sdust: sdust.cpp kalloc.o kalloc.h kdq.h kvec.h kseq.h ketopt.h sdust.h
	$(CXX) -D_SDUST_MAIN $(CXXFLAGS) $< kalloc.o -o $@ -lz

# SSE目标（使用C++编译器）
ifeq ($(arm_neon),)
ksw2_ll_sse.o: ksw2_ll_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse2 $(CPPFLAGS) $(INCLUDES) $< -o $@
endif

ksw2_extz2_sse41.o: ksw2_extz2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH $(INCLUDES) $< -o $@

ksw2_extd2_sse41.o:ksw2_extd2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH $(INCLUDES) $< -o $@

ksw2_exts2_sse41.o:ksw2_exts2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH $(INCLUDES) $< -o $@

ksw2_extz2_sse2.o:ksw2_extz2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse2 -mno-sse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH -DKSW_SSE2_ONLY $(INCLUDES) $< -o $@

ksw2_extd2_sse2.o:ksw2_extd2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse2 -mno-sse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH -DKSW_SSE2_ONLY $(INCLUDES) $< -o $@

ksw2_exts2_sse2.o:ksw2_exts2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) -msse2 -mno-sse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH -DKSW_SSE2_ONLY $(INCLUDES) $< -o $@

ksw2_dispatch.o:ksw2_dispatch.cpp ksw2.h
	$(CXX) -c $(CXXFLAGS) -msse4.1 $(CPPFLAGS) -DKSW_CPU_DISPATCH $(INCLUDES) $< -o $@

# NEON-specific targets on ARM

ksw2_extz2_neon.o:ksw2_extz2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -DKSW_SSE2_ONLY -D__SSE2__ $(INCLUDES) $< -o $@

ksw2_extd2_neon.o:ksw2_extd2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -DKSW_SSE2_ONLY -D__SSE2__ $(INCLUDES) $< -o $@

ksw2_exts2_neon.o:ksw2_exts2_sse.cpp ksw2.h kalloc.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -DKSW_SSE2_ONLY -D__SSE2__ $(INCLUDES) $< -o $@
# 其他SSE/NEON目标类似修改...

clean:
	rm -fr gmon.out *.o a.out $(PROG) $(PROG_EXTRA) *~ *.a *.dSYM build dist mappy*.so mappy.cpp python/mappy.cpp mappy.egg*

depend:
	(LC_ALL=C; export LC_ALL; makedepend -Y -- $(CXXFLAGS) $(CPPFLAGS) -- *.cpp)