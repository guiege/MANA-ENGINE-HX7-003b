#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform bool useTexture;

void main()
{
    if (useTexture)
        gl_FragColor = spriteColor * texture(image, TexCoords);
    else
        gl_FragColor = spriteColor;
}