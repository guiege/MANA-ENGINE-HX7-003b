#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;

uniform mat4 projection;

out vec4 vertexColor;
out vec2 v_TexCoord;
out float v_TexIndex;

void main()
{
    vertexColor = color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    gl_Position = projection * vec4(aPos, 1.0);
}