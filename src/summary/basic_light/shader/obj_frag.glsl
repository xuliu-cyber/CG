#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {  
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct Flashlight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
};

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 viewPos;
uniform Material material;
uniform Light light1;
uniform Light light2;
uniform Light light3;
uniform Flashlight flashlight;
uniform bool flashlighton=false;

void main() {
    //光线衰减
    float distance1    = length(light1.position - FragPos);
    float attenuation1 = 1.0 / (light1.constant + light1.linear * distance1 + 
                light1.quadratic * (distance1 * distance1));
    float distance2    = length(light2.position - FragPos);
    float attenuation2 = 1.0 / (light2.constant + light2.linear * distance2 + 
                light2.quadratic * (distance2 * distance2));
    float distance3    = length(light1.position - FragPos);
    float attenuation3 = 1.0 / (light3.constant + light3.linear * distance3 + 
                light3.quadratic * (distance3 * distance3));

    //环境光照
    vec3 ambient1 = light1.ambient * material.ambient;
    vec3 ambient2 = light2.ambient * material.ambient;
    vec3 ambient3 = light3.ambient * material.ambient;

    //漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(light1.position - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 =light1.diffuse * diff1 * material.diffuse;
    vec3 lightDir2 = normalize(light2.position - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 =light2.diffuse * diff2 * material.diffuse;
    vec3 lightDir3 = normalize(light3.position - FragPos);
    float diff3 = max(dot(norm, lightDir3), 0.0);
    vec3 diffuse3 =light3.diffuse * diff3 * material.diffuse;

    //镜面反射光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir1 = reflect(-lightDir1, norm);  
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), material.shininess);
    vec3 specular1 = light1.specular * spec1 * material.specular;  

    vec3 reflectDir2 = reflect(-lightDir2, norm);  
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), material.shininess);
    vec3 specular2 = light2.specular * spec2 * material.specular;  
    
    vec3 reflectDir3 = reflect(-lightDir3, norm);  
    float spec3 = pow(max(dot(viewDir, reflectDir3), 0.0), material.shininess);
    vec3 specular3 = light3.specular * spec3 * material.specular;  

    vec3 result = attenuation1*(ambient1 +diffuse1 +specular1) +attenuation2*(ambient2 +diffuse2 +specular2)+attenuation3*(ambient3 +diffuse3 +specular3);
   
    //手电筒边缘软化
    float theta = dot(normalize(flashlight.position - FragPos), normalize(-flashlight.direction));
    if(flashlighton && theta>flashlight.outerCutOff){
        float epsilon = (flashlight.cutOff - flashlight.outerCutOff);
        float flashintensity = clamp((theta - flashlight.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 flashambient = vec3(0.3,0.3,0.3) * material.ambient;
        vec3 flashlightDir = normalize(flashlight.position - FragPos);
        float flashdiff = max(dot(norm, flashlightDir), 0.0);
        vec3 flashdiffuse =vec3(0.3,0.3,0.3) * flashdiff * material.diffuse;
        vec3 reflectDir = reflect(-flashlightDir, norm);  
        float flashspec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 flashspecular = vec3(0.3,0.3,0.3) * flashspec * material.specular;  

        result+=flashintensity*(flashambient +flashdiffuse +flashspecular);
    }
    

    //总光照

    FragColor = vec4(result, 1.0);
}