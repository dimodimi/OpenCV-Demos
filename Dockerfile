FROM ubuntu:18.04
MAINTAINER Dimitrios Dimopoulos <dimodimi42@gmail.com>

RUN apt-get update \
    && apt-get install -y \
        build-essential \
        python3-pip \
	cmake \
        git \
        wget \
        unzip \
        yasm \
        pkg-config \
        libswscale-dev \
        libtbb2 \
        libtbb-dev \
        libjpeg-dev \
        libpng-dev \
        libtiff-dev \
        libavformat-dev \
        libpq-dev \
        libgtk2.0-dev \
	tesseract-ocr \
        libtesseract-dev \
        tesseract-ocr-eng \
        tesseract-ocr-script-latn \
        tesseract-ocr-ell \
        tesseract-ocr-script-grek \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install numpy

WORKDIR /
ADD https://www.dropbox.com/s/g8pjzv2de9gty8g/TextBoxes_icdar13.caffemodel?dl=1 .
RUN git clone https://github.com/opencv/opencv_contrib.git
RUN git clone https://github.com/opencv/opencv.git \
&& mkdir opencv/build \
&& cd opencv/build \
&& cmake -DBUILD_TIFF=ON \
  -DBUILD_opencv_java=OFF \
  -DOPENCV_ENABLE_NONFREE=ON \
  -DOPENCV_EXTRA_MODULES_PATH=/opencv_contrib/modules \
  -DWITH_CUDA=OFF \
  -DWITH_OPENGL=ON \
  -DWITH_OPENCL=ON \
  -DWITH_IPP=ON \
  -DWITH_TBB=ON \
  -DWITH_EIGEN=ON \
  -DWITH_V4L=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DCMAKE_BUILD_TYPE=RELEASE \
  -DCMAKE_INSTALL_PREFIX=$(python3.6 -c "import sys; print(sys.prefix)") \
  -DPYTHON_EXECUTABLE=$(which python3.6) \
  -DPYTHON_INCLUDE_DIR=$(python3.6 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
  -DPYTHON_PACKAGES_PATH=$(python3.6 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
  .. \
&& make -j4 install
