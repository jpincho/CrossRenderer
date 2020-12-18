#version 400
layout ( triangles ) in;

layout ( line_strip, max_vertices = 8 ) out;

uniform float u_NormalLength = 0.3;
uniform mat4 u_MVPMatrix;
uniform vec4 u_StartNormalColor = vec4 ( 1, 0, 0, 1 ), u_EndNormalColor = vec4 ( 0, 1, 0, 1 );
uniform int u_ShowVertexNormals = 1, u_ShowFaceNormals = 1;
in vec3 v_Normal[];

out vec4 v_Color;

void main()
    {
    if ( u_ShowVertexNormals == 1 )
        {
        int i;
        for ( i = 0; i < gl_in.length(); i++ )
            {
            vec3 P = gl_in[i].gl_Position.xyz;
            vec3 N = v_Normal[i].xyz;

            gl_Position = u_MVPMatrix * vec4 ( P, 1.0 );
            v_Color = u_StartNormalColor;
            EmitVertex();

            gl_Position = u_MVPMatrix * vec4 ( P + N * u_NormalLength, 1.0 );
            v_Color = u_EndNormalColor;
            EmitVertex();
            EndPrimitive();
            }
        }

    if ( u_ShowFaceNormals == 1 )
        {
        vec3 P0 = gl_in[0].gl_Position.xyz;
        vec3 P1 = gl_in[1].gl_Position.xyz;
        vec3 P2 = gl_in[2].gl_Position.xyz;

        vec3 V0 = P0 - P1;
        vec3 V1 = P2 - P1;

        vec3 N = cross ( V1, V0 );
        N = normalize ( N );

        // Center of the triangle
        vec3 P = ( P0 + P1 + P2 ) / 3.0;

        gl_Position = u_MVPMatrix * vec4 ( P, 1.0 );
        v_Color = u_StartNormalColor;
        EmitVertex();

        gl_Position = u_MVPMatrix * vec4 ( P + N * u_NormalLength, 1.0 );
        v_Color = u_EndNormalColor;
        EmitVertex();
        EndPrimitive();
        }
    }
