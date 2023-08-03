#version 400
uniform mat4 u_MVP[100];

in vec3 a_VertexPosition;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
    {
    gl_Position = u_MVP[gl_InstanceID] * vec4 ( a_VertexPosition, 1.0f );
    v_TexCoord = a_TexCoord;
    }