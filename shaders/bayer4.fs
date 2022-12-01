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

const int bayer4[4] = int[](0,  2,  
                            3,  1);
void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diffuse = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    //diffuse = diffuse+0.08;

    //position in matrix grid
    int column = int(mod(gl_FragCoord.x, 2));
    int row = int(mod(gl_FragCoord.y, 2));
    float threshold = float(bayer4[ column + 2 * row ]) / 4.0;
    
    if(diffuse>threshold) color=vec4(highColor,1.0);
    else color = vec4(lowColor,1.0);
}