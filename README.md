# Return of the One Bit

3D rendering showcase of dithering shaders inspired by the game *Return of the Obra Dinn* by Lucas Pope, which features two-color dithering graphics.


![Demo](images/demo.gif)


## Dithering

On a black-and-white two color system, shades of grey can be expressed in a pointillistic matter by juxtaposition of black and white pixels. This is a technique originating from newspapers and the printing industry.

This program showcases implementations of the following dithering methods.

![DitheringMethods](images/ditheringMethods.png)

### Threshold and Random Dithering

Threshold dithering is a method that compares each pixel value against a fixed threshold. If the value is larger than the threshold value the pixel falue is set to high, and if it is lower than the threshold value the pixel value is set to low. (In a two color system.)


Screenshot from Program : 
![Threshold](images/thresholdScreenshot.png)

Random dithering, the first method made to improve threshold dithering, decides pixel values by making the threshold value random at each pixel. Random dithering produces a very noisy image.


Screenshot from Program :
![Random](images/randomScreenshot.png)

## Ordered Dithering

Ordered dithering dithers using a *dither matrix*. For each pixel, the value of the pattern at the corresponding location is used as a threshold for the pixel.

Halftone dithering

Bayer dithering

![BayerScale](images/bayerScale.png)



Screenshot from Program (Using 4x4 Bayer Matrix):
![Bayer4x4](images/bayer4.png)


Screenshot from Program (Using 16x16 Bayer Matrix):
![Bayer16x16](images/bayer16.png)


Screenshot from Program (Using 64x64 Bayer Matrix):
![Bayer64x64](images/bayer64.png)

## Dependencies

Written in C++ using OpenGL and GLFW3.

Used ImGui for interface purposes.

Used Assimp for loading 3D models.

Built using vcpkg on Windows.

## References

Images and algorithms are referenced from [wikipedia](https://en.wikipedia.org/wiki/Dither#Algorithms).

3D models from [common-3D-test-models](https://github.com/alecjacobson/common-3d-test-models) uploaded by alecjacobson.