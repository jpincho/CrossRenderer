#version 400
in vec3 a_Position;
out vec3 v_TexCoord;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main()
    {
    v_TexCoord = a_Position;
    vec4 pos = u_ProjectionMatrix * u_ViewMatrix * vec4 ( a_Position, 1.0 );
    gl_Position = pos.xyww;
    }
