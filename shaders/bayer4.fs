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
    float diff = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    diff+=0.08f;

}