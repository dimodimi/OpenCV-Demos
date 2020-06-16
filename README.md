# Computer Vision demos using OpenCV library

The programs were tested with the docker image described in the provided Dockerfile. To build the image with a given name "img_name", run in your local terminal:
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

### Harris corner detector 
A basic implementation of the Harris corner detector

![equation](https://latex.codecogs.com/gif.latex?\begin{align*}&space;E(u,&space;v)&space;&=&space;\sum_{(x,&space;y)}&space;w(x,&space;y)&space;\cdot&space;\left[&space;I(x&plus;u,&space;y&plus;v)&space;-&space;I(x,y)&space;\right&space;]^2&space;\\&space;&=&space;\sum_{(x,&space;y)}&space;w(x,&space;y)&space;\cdot&space;\left(&space;uI_x&space;-&space;vI_y&space;\right&space;)^2&space;\\&space;&=&space;\sum_{(x,y)}&space;w(x,&space;y)&space;\begin{bmatrix}&space;u&v&space;\end{bmatrix}&space;\begin{bmatrix}&space;I_x^2&space;&&space;I_{xy}&space;\\&space;I_{xy}&space;&&space;I_y^2&space;\end{bmatrix}&space;\begin{bmatrix}&space;u&space;\\&space;v&space;\end{bmatrix}&space;\\&space;&=&space;\begin{bmatrix}&space;u&v&space;\end{bmatrix}&space;\begin{bmatrix}&space;\sum_{(x,y)}&space;w(x,y)I_x^2&space;&&space;\sum_{(x,y)}&space;w(x,y)I_{xy}&space;\\&space;\sum_{(x,y)}&space;w(x,y)I_{xy}&space;&&space;\sum_{(x,y)}&space;w(x,y)I_{y^2}\end{bmatrix}&space;\begin{bmatrix}&space;u&space;\\&space;v&space;\end{bmatrix}&space;\\&space;&=&space;\begin{bmatrix}&space;u&v&space;\end{bmatrix}&space;M&space;\begin{bmatrix}&space;u&space;\\&space;v&space;\end{bmatrix}&space;\end{align*})

where (u, v) are the shifts around a point p in the image, (x, y) are in a neighborhood around p and I_x, I,y are the derivatives of the image at the point p in the horizontal and vertical direction.

In short, the algorithm steps are:
1. Computing the derivatives by filtering the image with the Sobel kernel
2. Computing the per-element products of the derivatives
3. Filtering the products with a Gaussian kernel and constructing the M matrix
4. Compute the response of the detector R (shown below)
5. Apply non-maximum suppression to yield the corners
