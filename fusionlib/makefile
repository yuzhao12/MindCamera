
# 1、准备工作，编译方式、目标文件名、依赖库路径的定义。
CC = g++
CFLAGS  := -Wall -O3 -std=c++0x 

# opencv 头文件和lib路径 
OPENCV_INC_ROOT = /usr/local/include/opencv2
OPENCV_LIB_ROOT = /usr/local/lib
SUPERLU_LIB_ROOT =../lib
SUPERLU_INC_ROOT =../inc
INCLUDE = ./
OBJS = fusion.o #.o文件与.cpp文件同名
LIB = libfusion.so # 目标文件名 

OPENCV_INC= -I $(OPENCV_INC_ROOT)

SUPERLU_INC = -I$(SUPERLU_INC_ROOT)


INCLUDE_PATH = $(OPENCV_INC)

LIB_PATH = -L $(OPENCV_LIB_ROOT)

SINCLUDE_PATH = $(SUPERLU_INC)

SLIB_PATH = -L $(SUPERLU_LIB_ROOT)



# 依赖的lib名称
OPENCV_LIB = -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lippicv -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core

SUPERLU_LIB = -lsuperlu_4.3
all : $(LIB)

# 2. 生成.o文件 
%.o : %.cpp possion.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(INCLUDE_PATH) $(SINCLUDE_PATH) -I$(INCLUDE)
possion.o : possion.cpp possion.h
	$(CC) $(CFLAGS) -fPIC -c possion.cpp -o possion.o $(INCLUDE_PATH) $(SINCLUDE_PATH) -I$(INCLUDE)
# 3. 生成动态库文件
$(LIB) : $(OBJS) possion.o
	rm -f $@
	g++ -shared -fPIC -o $@ $(OBJS) possion.o $(LIB_PATH) $(OPENCV_LIB) $(SLIB_PATH) $(SUPERLU_LIB) 
	rm -f $(OBJS) possion.o

tags :
	ctags -R *
install :
	cp libfusion.so ../lib/

# 4. 删除中间过程生成的文件 
clean:
	rm -f $(OBJS) $(TARGET) $(LIB) possion.o
