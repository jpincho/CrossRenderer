#version 400
in vec3 a_VertexPosition;
in vec2 a_TexCoord;
in vec3 a_Normal;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ModelTransposeInverse;
uniform mat4 u_MVPMatrix;

void main()
    {
    v_FragPos = vec3 ( u_ModelMatrix * vec4 ( a_VertexPosition, 1.0f ) );
    v_Normal = mat3 ( u_ModelTransposeInverse ) * a_Normal;
    gl_Position = u_MVPMatrix * vec4 ( a_VertexPosition, 1.0 );
    v_TexCoord = a_TexCoord;
    }
