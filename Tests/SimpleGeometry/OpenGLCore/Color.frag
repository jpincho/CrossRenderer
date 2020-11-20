#version 400
out vec4 FragOutput;

uniform vec4 u_Color = vec4(1,0,0,1);

void main()
    {
    FragOutput = u_Color;
    }
