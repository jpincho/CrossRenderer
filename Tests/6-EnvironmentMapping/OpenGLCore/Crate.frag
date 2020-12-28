#version 330 core
out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_ViewPosition;
uniform samplerCube u_SkyboxTexture;
#define REFRACTION
void main()
    {
    vec3 I = normalize ( v_FragPos - u_ViewPosition );

#if defined REFRACTION
    float ratio = 1.00 / 1.52;
    vec3 R = refract ( I, normalize ( v_Normal ), ratio );
#else
    vec3 R = reflect ( I, normalize ( v_Normal ) );
#endif
    FragColor = vec4 ( texture ( u_SkyboxTexture, R ).rgb, 1.0 );
    }