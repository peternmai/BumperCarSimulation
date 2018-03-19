#version 330 core

// Inputs sent over from vertex shader
in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 vertexPosition;
in mat4 modelView;

// Variables passed in from CPU
struct Material {
    vec3 color;
    float ambientCoefficient;
    float diffuseCoefficient;
    float specularCoefficient;
    float shininessConstant;
};

struct DirectionalLightSource {
    vec3 color;
    vec3 direction;
};

uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float shininess;
uniform vec3 lightColor;
uniform vec3 lightDirection;

// Output the color of the current fragment
out vec4 color;

void main()
{
    // Create the material struct
    Material material;
    material.color = vertexColor;
    material.ambientCoefficient = ambient;
    material.diffuseCoefficient = diffuse;
    material.specularCoefficient = specular;
    material.shininessConstant = shininess;
    
    // Create Directional Light Struct
    DirectionalLightSource directionalLight;
    directionalLight.color = lightColor;
    directionalLight.direction = normalize(vec3((transpose(inverse(modelView))) * vec4(lightDirection, 0.0f)));
    
    // Calculate the normals and position in world view
    vec3 normal = normalize(vec3((transpose(inverse(modelView))) * vec4(vertexNormal, 0.0f)));
    vec3 fragPosition = vec3(modelView * vec4(vertexPosition, 1.0f));

    // Set ambient color
    color = vec4(material.color, 1.0f) * material.ambientCoefficient;
    
    // Factor in ambient component of directional light
    vec3 dl_ambient = directionalLight.color * material.ambientCoefficient;
    
    // Factor in diffuse component from directional light
    vec3 dl_lightDirection = normalize(-1.0f * directionalLight.direction);
    vec3 dl_diffuse = material.diffuseCoefficient * max(dot(dl_lightDirection, normal), 0.0) * directionalLight.color;
    
    // Factor in specular component from directional light
    vec3 dl_reflectionDirection = reflect(-1.0f * dl_lightDirection, normal);
    float dl_specTemp = pow(max(dot(dl_reflectionDirection, normalize(-1.0f * fragPosition)), 0.0), material.shininessConstant);
    vec3 dl_specular = material.specularCoefficient * dl_specTemp * directionalLight.color;

    // Color after factoring in directional light
    color += vec4((dl_ambient + dl_diffuse + dl_specular) * material.color, 0.0f);
    //color = vec4(directionalLight.direction, 1.0f);
}
