#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 highColor;
uniform vec3 lowColor;

uniform float diffuseScale;

void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diff = diffuseScale * max(dot(normal, lightDir), 0.0); //cos

    //ambient
    diff+=0.08f;

    if(diff>0.5f)color = vec4(highColor,1.0f);
    else color = vec4(lowColor,1.0f);

}