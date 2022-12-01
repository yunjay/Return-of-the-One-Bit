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
const int bayer16[16] = int[16](0,  8,  2,  10, 
                                12, 4,  14, 6, 
                                3,  11, 1,  9, 
                                15, 7,  13, 5);
float ditherIndex() { 
    //value from dither matrix
    //also uses gl_FragCoord
    //gl_FragCoord % 4 to split the screen into blocks for 4x4 squares
    //glsl mod function for % as % is not implemented as an operator is glsl
    
    int column = int(mod(gl_FragCoord.x, 4));
    int row = int(mod(gl_FragCoord.y, 4));
    return (bayer16[ (column + 4 * row) ] / 16.0);
}
void main() {
    //diffuse
    vec3 normal = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos);
    float diffuse = diffuseScale * max(dot(normal, lightDir), 0.0); //cos
    
    //ambient
    diffuse = diffuse+0.08;

    //decide color
    vec3 closeColor;
    vec3 otherColor;
    float closeColorf;
    if(diffuse > 0.5) {
        closeColor = highColor;
        otherColor = lowColor;
        closeColorf = 1.0;
    }
    else{
        closeColor = lowColor;
        otherColor = highColor;
        closeColorf = 0.0;
    }
    float dither = ditherIndex();
    float dist = abs(closeColorf - diffuse);
    
    if (dist < dither ) color = vec4(closeColor,1.0f) else color = vec4(otherColor,1.0f);
}