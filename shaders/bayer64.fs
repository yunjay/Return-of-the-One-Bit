#version 430

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 highColor;
uniform vec3 lowColor;

uniform vec2 resolution;

uniform float diffuseScale;
//threshold map
const int bayer64[64] = int[64](0,  32,  8,  40, 2, 34, 10, 42,
                                48, 16, 56, 25, 50, 18, 58, 26,
                                12, 44, 4, 36, 14, 46, 6, 38,
                                60, 28, 52, 20, 62, 30, 54, 22,
                                3, 35, 11, 43, 1, 33, 8, 41,
                                51, 19, 59, 27, 49, 17, 57, 25,
                                15, 47, 7, 39, 13, 45, 5, 37,
                                63, 31, 55, 23, 61, 29, 53, 21
                                );

void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diffuse = diffuseScale * max(dot(normal, lightDir), 0.0); //cos

    //position in matrix grid
    int column = int(mod(gl_FragCoord.x, 8));
    int row = int(mod(gl_FragCoord.y, 8));
    float threshold = float(bayer64[ column + 8 * row ]) / 64.0;
    
    if(diffuse>threshold) color=vec4(highColor,1.0);
    else color = vec4(lowColor,1.0);
}