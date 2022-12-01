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

const int halftone[36] = int[36](   35, 30, 18, 22, 31, 36,  
                                    29, 15, 10, 17, 21, 32,
                                    14, 9, 5, 6, 16, 20,
                                    13, 4, 1, 2, 11, 19,
                                    28, 8, 3, 7, 24, 25,
                                    34, 27, 12, 23, 26, 33  );
void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diffuse = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    //diffuse = diffuse+0.08;

    //position in matrix grid
    int column = int(mod(gl_FragCoord.x, 6));
    int row = int(mod(gl_FragCoord.y, 6));
    float threshold = float(halftone[ column + 6 * row ]) / 36.0;
    
    if(diffuse>threshold) color=vec4(highColor,1.0);
    else color = vec4(lowColor,1.0);
}