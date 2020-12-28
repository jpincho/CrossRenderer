#version 330 core
out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_ViewPosition;
uniform samplerCube u_SkyboxTexture;
uniform int u_UseRefraction;
uniform float u_RefractionFactor;

void main()
    {
    vec3 I = normalize ( v_FragPos - u_ViewPosition );
    vec3 R;
    if ( u_UseRefraction )
        R = refract ( I, normalize ( v_Normal ), u_RefractionFactor );
    else
        R = reflect ( I, normalize ( v_Normal ) );

    FragColor = vec4 ( texture ( u_SkyboxTexture, R ).rgb, 1.0 );
    }