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
const int bayer16[16] = int[](
                            0,  8,  2,  10,
                            12, 4,  14, 6,
                            3,  11, 1,  9,
                            15, 7,  13, 5);
float ditherIndex() { //value from dither matrix
    //also uses gl_FragCoord
    //gl_FragCoord % 4 to split the screen into blocks for 4x4 squares
    //glsl mod function for % as % is not implemented as an operator is glsl
    int column = int(mod(gl_FragCoord.x, 4));
    int row = int(mod(gl_FragCoord.y, 4));
    return bayer16[(column + 4 * row)] / 16.0;
}
void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diff = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    diff+=0.08f;

    //decide color
    float closeColor = (diff < 0.5) ? lowColor : highColor ;
    float otherColor = 1 - closeColor;
    float d = ditherIndex();
    float dist = abs(closeColor - diff);
    return (dist < d) ? closeColor : otherColor;
    
    if(diff>randomThreshold)color = vec4(highColor,1.0f);
    else color = vec4(lowColor,1.0f);

}