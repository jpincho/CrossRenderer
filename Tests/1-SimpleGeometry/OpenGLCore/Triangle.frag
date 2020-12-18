#version 400
out vec4 FragOutput;
in vec3 v_Color;

void main()
    {
    FragOutput = vec4(v_Color, 1.0);
    }
