#version 330 core

// Inputs sent over from vertex shader
in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 FragPos;

in vec3 vertexPosition;
in mat4 modelView;
in vec4 shadowCoordinate;

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
uniform float drawShadow;

// Linear Fog
uniform int fog;

// Toon Shading
uniform vec3 eyePos;
uniform int toonShading;

// Shadow map
uniform sampler2D shadowMap;

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

	float diff = material.diffuseCoefficient * max(dot(dl_lightDirection, normal), 0.0);

	// TOON SHADING
	if(toonShading == 1) {
		if(diff < 0.2) diff = 0.0;
		else if(diff < 0.5) diff = 0.5;
		else if(diff < 0.8) diff = 0.8;
		else diff = 1;
	}

    vec3 dl_diffuse = diff * directionalLight.color;


    
    // Factor in specular component from directional light
    vec3 dl_reflectionDirection = reflect(-1.0f * dl_lightDirection, normal);
    float dl_specTemp = pow(max(dot(dl_reflectionDirection, normalize(-1.0f * fragPosition)), 0.0), material.shininessConstant);

	float spec = material.specularCoefficient * dl_specTemp;
	// TOON SHADING
	/*
	if(toonShading == 1) {
		if(spec < 0.2) spec = 0.2;
		else if(spec < 0.5) spec = 0.5;
		else if(spec < 0.8) spec = 0.8;
		else spec = 1;
	}
	*/
    vec3 dl_specular = spec * directionalLight.color;



    // Color after factoring in directional light
    color += vec4((dl_ambient + dl_diffuse + dl_specular) * material.color, 0.0f);

    // Poisson Sampling Constant
    vec2 poissonDisk[4] = vec2[](
                                 vec2( -0.94201624, -0.39906216 ),
                                 vec2( 0.94558609, -0.76890725 ),
                                 vec2( -0.094184101, -0.92938870 ),
                                 vec2( 0.34495938, 0.29387760 )
                                 );

    // Calculate the shadow
    float visibility = 1.0f;
    float bias = 0.005; //max(0.005f, 0.02f * (1.0f - dot(normal, directionalLight.direction)));
    for(int i = 0; i < 4; i++) {
        if( texture(shadowMap, shadowCoordinate.xy + poissonDisk[i] / 1000.0f).r < (shadowCoordinate.z-bias) )
            visibility = visibility - 0.15f;
    }

    // Account for shadow
    if( drawShadow == 1.0f )
        color *= visibility;
	
	
	// toon shading
	
	if(toonShading == 1) {
		float edge = max(0, dot(normalize(normal), normalize(eyePos - FragPos)));
		if(edge < 0.01) {
			vec3 tempBlack = vec3(0.0, 0.0, 0.0);
			color = vec4(tempBlack, 1.0);
		}
	}
	

	// Linear Shadows
    float fogEnd = 200.0f;
    float fogStart = 20.0f;
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    if(fog == 1) {
      vec3 colorwfog = vec3(0.0, 0.0, 0.0);
      float fogFactor = (fogEnd - abs(FragPos.z)) / (fogEnd - fogStart);
      fogFactor = clamp (fogFactor, 0.0, 1.0);

	  vec3 tempColor = vec3(color);
      colorwfog = mix(fogColor, tempColor, fogFactor);

      color = vec4(colorwfog, 1.0);
    }
}
