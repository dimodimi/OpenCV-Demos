# Computer Vision demos using OpenCV library

The programs were tested with the docker image described in the provided Dockerfile. This pulls and builds the latest version of OpenCV and OpenCV_contrib (4.3.0 at the time of writing) To build the image with a given name "img_name", run in your local terminal:
```
docker build -t <img_name> <Dockerfile path>
```
You can then run a docker container based on the image:
```
docker run -it --rm -v $(realpath OpenCV_demos):/home/ --net=host --env="DISPLAY" --volume "$HOME/.Xauthority:/root/.Xauthority:rw" --device=/dev/video0:/dev/video0 <img_name> /bin/bash
```
This will allow us to access the OpenCV_demos directory from the container, launch windows through the hosts X-server and use the hosts default camera.
Note that to access the camera we need root access in the container. If the camera is not needed you can add the option "-u <host uid>:<mapped uid>" when executin docker run. A typical choice is "-u 1000:1000", running in the container as the host ID.

## Building source
```
mkdir build && cd build
cmake ..
make
```

## Demos

- [Harris corner detector](https://github.com/dimodimi/OpenCV_demos#harris-corner-detector)
- [Canny edge detector](https://github.com/dimodimi/OpenCV_demos#canny-edge-detector)
- [Motion detection](https://github.com/dimodimi/OpenCV_demos#motion-detection)

### Harris corner detector
A basic implementation of the Harris corner detector

![equation](https://latex.codecogs.com/gif.latex?%5Cbegin%7Balign*%7D%20E%28u%2C%20v%29%20%26%3D%20%5Csum_%7B%28x%2C%20y%29%7D%20w%28x%2C%20y%29%20%5Ccdot%20%5Cleft%5B%20I%28x&plus;u%2C%20y&plus;v%29%20-%20I%28x%2Cy%29%20%5Cright%20%5D%5E2%20%5C%5C%20%26%3D%20%5Csum_%7B%28x%2C%20y%29%7D%20w%28x%2C%20y%29%20%5Ccdot%20%5Cleft%28%20uI_x%20-%20vI_y%20%5Cright%20%29%5E2%20%5C%5C%20%26%3D%20%5Csum_%7B%28x%2Cy%29%7D%20w%28x%2C%20y%29%20%5Cbegin%7Bbmatrix%7D%20u%26v%20%5Cend%7Bbmatrix%7D%20%5Cbegin%7Bbmatrix%7D%20I_x%5E2%20%26%20I_%7Bxy%7D%20%5C%5C%20I_%7Bxy%7D%20%26%20I_y%5E2%20%5Cend%7Bbmatrix%7D%20%5Cbegin%7Bbmatrix%7D%20u%20%5C%5C%20v%20%5Cend%7Bbmatrix%7D%20%5C%5C%20%26%3D%20%5Cbegin%7Bbmatrix%7D%20u%26v%20%5Cend%7Bbmatrix%7D%20%5Cbegin%7Bbmatrix%7D%20%5Csum_%7B%28x%2Cy%29%7D%20w%28x%2Cy%29I_x%5E2%20%26%20%5Csum_%7B%28x%2Cy%29%7D%20w%28x%2Cy%29I_%7Bxy%7D%20%5C%5C%20%5Csum_%7B%28x%2Cy%29%7D%20w%28x%2Cy%29I_%7Bxy%7D%20%26%20%5Csum_%7B%28x%2Cy%29%7D%20w%28x%2Cy%29I_%7By%5E2%7D%5Cend%7Bbmatrix%7D%20%5Cbegin%7Bbmatrix%7D%20u%20%5C%5C%20v%20%5Cend%7Bbmatrix%7D%20%5C%5C%20%26%3D%20%5Cbegin%7Bbmatrix%7D%20u%26v%20%5Cend%7Bbmatrix%7D%20M%20%5Cbegin%7Bbmatrix%7D%20u%20%5C%5C%20v%20%5Cend%7Bbmatrix%7D%20%5Cend%7Balign*%7D)

where (u, v) are the shifts around a point p in the image, (x, y) are in a neighborhood around p and I_x, I,y are the derivatives of the image at the point p in the horizontal and vertical direction.

In short, the algorithm steps are:
1. Computing the derivatives by filtering the image with the Sobel kernel
2. Computing the per-element products of the derivatives
3. Filtering the products with a Gaussian kernel and constructing the M matrix
4. Compute the response of the detector R (shown below)
5. Apply non-maximum suppression to yield the corners

![equation](https://latex.codecogs.com/gif.latex?R%20%3D%20det%28M%29%20-%20k%20%5Ccdot%20%28tr%28M%29%29%5E2)

Apply the method by running:
```
cd build
./harris <img_path>
```

### Canny edge detector
Application of the Canny edge detector in a camera feed. The main steps are:
1. Denoise image by applying a Gaussian filter (might not be needed for cheaper cameras with soft lens)
2. Compute the gradient in polar coordinates
3. Apply non-maximum suppression to get rid of spurious response to edge detection
4. Apply double thresholding on the gradient values, categorizing them as non-edges, weak and strong edges
5. Iteratively determine if weak edges are connected to strong ones. These are promoted into strong edges, while the rest are discarded

To run the detector run the command below. The method is applied to your default camera feed. Use the trackbar to set the lower threshold value (the upper threshold is set to two times the lower threshold value).
```
cd build
./canny_test
```

### Motion Detection
A basic motion detction implementation.
1. Compute the intensity differense of two consecutive greyscale frames
2. Apply the canny edge detector and dilate the strong edges
3. Find the contours of the candidate movement regions
4. If the area of a contour is above a threshold then count the contour as valid movement and draw the best fitting ellipse around the area

We can adjust both the canny detector threshold and the contour area threshold via trackbars on the window. The demo can be used with either live feed from a camera (assumed located in /dev/video[cam_index]) or a video file provided by the user.
```
./motion_detector -type=cam -cam_index=<integer>(default: 0)
./motion_detector -type=vid -vid_filename=<path to video>
```
