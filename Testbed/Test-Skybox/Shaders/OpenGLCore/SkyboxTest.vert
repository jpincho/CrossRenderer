#version 400
in vec3 a_VertexPosition;
out vec3 v_TexCoord;

uniform mat4 u_MVP;

void main()
    {
#if 1
    v_TexCoord = a_VertexPosition;
#else
    v_TexCoord = vec3(a_VertexPosition.xy, -a_VertexPosition.z);// Flip Z?
#endif

#if 1
    vec4 pos = u_MVP * vec4 ( a_VertexPosition, 1.0 );
    gl_Position = pos.xyww;// Optimization. Must use depth test = LEQUAL
#else
    gl_Position = u_MVP * vec4 ( a_VertexPosition, 1.0 );
#endif
    }
