#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 highColor;
uniform vec3 lowColor;

uniform vec2 resolution;

uniform float diffuseScale;

float random (vec2 st) {
    //pseudorandom oneliner from book of shaders, returns 0.0~1.0
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}
void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diff = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    diff+=0.1f;

    //per pixel random threshold
    vec2 coord = gl_FragCoord.xy/resolution.xy;
    float randomThreshold = random( coord );
    
    if(diff>randomThreshold)color = vec4(highColor,1.0f);
    else color = vec4(lowColor,1.0f);

}