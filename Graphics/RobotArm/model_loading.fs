#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 ObjColor;
uniform bool hasTextures;

uniform Light light;
uniform bool spot;

void main()
{    
    vec3 fColor = hasTextures ? texture(texture_diffuse1, TexCoords).rgb : ObjColor;

    // ambient
    const float ambientStrength = 0.1;
    vec3 ambient = light.ambient * lightColor;
 
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.specular * diff * lightColor;
    
    // specular
    const float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32); //32
    vec3 specular = light.specular * spec * lightColor;  
        
    if(spot)
    {
        // spotlight (soft edges)
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;
    } 
        
    else 
    {
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation; 
     }
    

    vec3 result = (ambient + diffuse + specular) * fColor;
    FragColor = vec4(result, 1.0);
}