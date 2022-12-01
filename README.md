# Return of the One Bit

3D rendering showcase of dithering shaders inspired by the game *Return of the Obra Dinn* by Lucas Pope, which features two-color dithering graphics.

## Dithering

On a black-and-white two color system, shades of grey can be expressed in a pointillistic matter by juxtaposition of black and white pixels. This is a technique originating from newspapers and the printing industry.

This program showcases implementations of the following dithering methods.

![DitheringMethods](images/ditheringMethods.png)

### Ordered Dithering

![BayerScale](images/bayerScale.png)

## Dependencies

Written in C++ using OpenGL and GLFW3.

Used ImGui for interface purposes.

Used Assimp for loading 3D models.

Built using vcpkg on Windows.

## References

All images and algorithms are referenced from [wikipedia](https://en.wikipedia.org/wiki/Dither#Algorithms).

3D models from [common-3D-test-models](https://github.com/alecjacobson/common-3d-test-models) uploaded by alecjacobson.