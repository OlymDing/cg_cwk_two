# version 330 core

struct Material {
    // object color values & strength
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    // light color values & strength
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // vec3 position;
    vec3 direction;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main () {
    // pre-parameters
    // --------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos-light.direction);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ambient
    // -------
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    // -------
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * texture(material.diffuse, TexCoords)).rgb;

    // specular
    // --------
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);

    // final result
    // ------------
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
};