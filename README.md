# MindCamera

## Introduction
    
  This is a collection of code I've put together to realize an interactive image retrieval and synthesis stack. The stack provides users rich ways to access images, such as query by sketch or keyword. Besides, our user-friendly UI make it easy and funny to extract the foreground and blend it into the background. The frontend is compliant with the latest HTML5 standards, and many JavaScript frameworks are used. The backend allows system ad-ministrators create processing pipelines that can be adapted to a variety of clients. 
    
## Requirements

1. ubuntu14.04

2. python2.7
  - *flask*
  - *numpy*
  - *cv2(3.1.0)*

3. Opencv 3.1.0

4. SuperLu 4.3


## Installation

1. Installing Git Large File Storage from https://git-lfs.github.com/

2. Clone the MindCamera repository, we'll call the directory that you cloned MindCamera into `MC_ROOT`

```
git clone https://github.com/yuzhao12/MindCamera.git
```

3. Install the required python modules (We recommand to use [Anaconda](https://www.continuum.io/downloads))

```shell
conda install flask numpy opencv==3.1.0
```

4. Download and build [Opencv 3.1.0](http://opencv.org/releases.html)

5. Download and build [SuperLu 4.3](http://crd-legacy.lbl.gov/~xiaoye/SuperLU/), then copy libs and heards

```shell
cp SuperLU_4.3/lib/  $MC_ROOT/
cp SuperLU_4.3/SRC/*.h $MC_ROOT/inc/
```

6. Build fusionlib module (before that modify the path of Opencv in the `$MC_ROOT/ fusionlib/makefile`)
```
OPENCV_INC_ROOT = /usr/local/include/opencv2
OPENCV_LIB_ROOT = /usr/local/lib
```
```shell
make
make install
```

7. Build  sbirlib module (before that modify the path of Opencv in the `$MC_ROOT/sbirlib/makefile`)

```
OPENCV_INC_ROOT = /usr/local/include/opencv2
OPEN_IN=/usr/local/include/opencv
OPENCV_LIB_ROOT = /usr/local/lib
PY_INCLUDE = /home/zhaoyu/anaconda2/include/python2.7
```
```shell
make
make install
```

8. Start MindCamera 

```
cd $MC_ROOT/
python run.py
```


## Fast Installation

A Docker of MindCamera is released on [Docker Hub](https://hub.docker.com/r/buptzhaoyu/imagecombine/)

