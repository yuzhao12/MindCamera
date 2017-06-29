
# 1、准备工作，编译方式、目标文件名、依赖库路径的定义。
CC = g++
CFLAGS  := -Wall -O3 -std=c++0x 

# opencv 头文件和lib路径 
OPENCV_INC_ROOT = /usr/local/include/opencv2
OPEN_IN=/usr/local/include/opencv
OPENCV_LIB_ROOT = /usr/local/lib
SUPERLU_LIB_ROOT =../lib
SUPERLU_INC_ROOT =../inc
INCLUDE = ./
PY_INCLUDE = /home/zhaoyu/anaconda2/include/python2.7
OBJS = SGF-HOG-querytest.o GFHOG.o HogDetect.o BOWHistogram.o #.o文件与.cpp文件同名
LIB = review.so # 目标文件名 

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
SGF-HOG-querytest.o : SGF-HOG-querytest.cpp GFHOG.h BOWHistogram.h
	$(CC) $(CFLAGS) -fPIC -c $< -fpermissive -o $@ $(INCLUDE_PATH) -I$(INCLUDE) -I$(PY_INCLUDE)
GFHOG.o : GFHOG.cpp GFHOG.h HogDetect.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(INCLUDE_PATH) $(SINCLUDE_PATH) -I$(INCLUDE)
HogDetect.o : HogDetect.cpp HogDetect.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(INCLUDE_PATH) -I$(INCLUDE)
BOWHistogram.o : BOWHistogram.cpp BOWHistogram.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(INCLUDE_PATH) -I$(INCLUDE) -I$(OPEN_IN)
# 3. 生成动态库文件
$(LIB) : $(OBJS)
	rm -f $@
	g++ -shared -fPIC -o $@ $^ $(LIB_PATH) $(OPENCV_LIB) $(SLIB_PATH) $(SUPERLU_LIB) 
	rm -f $^

tags :
	ctags -R *
install :
	cp review.so ../flaskr2/

# 4. 删除中间过程生成的文件 
clean:
	rm -f $(OBJS) $(LIB)
