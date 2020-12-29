#version 400
out vec4 FragOutput;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform sampler2D u_DiffuseTexture, u_SpecularTexture;
uniform float u_Shininess;
uniform vec3 u_ViewPosition;

struct AmbientLight
    {
    vec3 Color;
    float Intensity;
    };

uniform AmbientLight u_AmbientLight;

layout ( std140 ) uniform u_PointLightBlock
    {
    vec4 Position;
    vec4 Diffuse, Specular;
    float Intensity;
    float ConstantAttenuation;
    float LinearAttenuation;
    float ExponentialAttenuation;
    } u_PointLights[2];

vec3 CalculatePointLightContribution ( int LightIndex, vec3 ViewPosToFrag, vec3 Normal, vec3 MaterialDiffuse, vec3 MaterialSpecular, float MaterialShininess )
    {
    vec3 LightPosToFrag = u_PointLights[LightIndex].Position.xyz - v_FragPos;
    // attenuation
    float distance = length ( LightPosToFrag ); // Light to fragment vector
    float attenuation = u_PointLights[LightIndex].ConstantAttenuation + u_PointLights[LightIndex].LinearAttenuation * distance + u_PointLights[LightIndex].ExponentialAttenuation * pow ( distance, 2.0f );

    // diffuse
    vec3 NormalizedLightToFrag = normalize ( LightPosToFrag );// normalize, and use just the direction
    float DiffuseComponent = max ( dot ( Normal, NormalizedLightToFrag ), 0.0 ); // Calculate dot product on the angle between the two vectors, and make it positive

    // specular
    vec3 NormalizedViewPosToFrag = normalize ( ViewPosToFrag );
    vec3 halfwayDir = normalize ( NormalizedLightToFrag + NormalizedViewPosToFrag );
    float SpecularComponent = pow ( max ( dot ( Normal, halfwayDir ), 0.0 ), MaterialShininess );

    // add it all together
    vec3 DiffuseContribution = DiffuseComponent * u_PointLights[LightIndex].Diffuse.xyz * MaterialDiffuse * u_PointLights[LightIndex].Intensity;
    vec3 SpecularContribution = SpecularComponent * u_PointLights[LightIndex].Specular.xyz * MaterialSpecular * u_PointLights[LightIndex].Intensity;
    DiffuseContribution /= attenuation;
    SpecularContribution /= attenuation;
    return DiffuseContribution + SpecularContribution;
    }

void main()
    {
    vec3 TotalPointLightContribution = vec3 ( 0.0f, 0.0f, 0.0f );

    vec4 Diffuse = texture ( u_DiffuseTexture, v_TexCoord );
    vec3 Specular = texture ( u_SpecularTexture, v_TexCoord ).xyz;

    TotalPointLightContribution += CalculatePointLightContribution ( 0,
                                   u_ViewPosition - v_FragPos,
                                   v_Normal,
                                   Diffuse.xyz,
                                   Specular,
                                   u_Shininess );

    TotalPointLightContribution += CalculatePointLightContribution ( 1,
                                   u_ViewPosition - v_FragPos,
                                   v_Normal,
                                   Diffuse.xyz,
                                   Specular,
                                   u_Shininess );


    FragOutput.xyz = TotalPointLightContribution;
    // Ambient light is calculated here, so it only adds once to the fragment, instead of once every light
    vec3 AmbientLightContribution = u_AmbientLight.Color * u_AmbientLight.Intensity * Diffuse.xyz;
    FragOutput.xyz += AmbientLightContribution;
    FragOutput.a = Diffuse.a ;
    }
