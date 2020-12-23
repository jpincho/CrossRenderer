#version 400
out vec4 FragOutput;

in vec2 v_TexCoord;
in vec4 v_Color;
uniform sampler2D u_Texture;

void main()
    {
    FragOutput = v_Color * texture ( u_Texture, v_TexCoord );
    }