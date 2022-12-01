#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 highColor;
uniform vec3 lowColor;

void main() {
    //ambient
    vec3 ambient = 0.04 * highColor;
    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0); //cos
    vec3 diffuse = 1.0 * diff * highColor;
    
    //specular
    vec3 specular;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    if(dot(lightDir,normal)>=0){
        specular = 0.03 * spec * highColor;
	} else specular =vec3(0,0,0); 
    color = vec4(lowColor + ambient + diffuse + specular, 1.0f);
}