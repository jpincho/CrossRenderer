#version 400
out vec4 FragColor;
in vec3 v_TexCoord;
uniform samplerCube u_SkyboxTexture;

void main()
    {
    FragColor = texture ( u_SkyboxTexture, v_TexCoord );
    }