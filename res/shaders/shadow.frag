#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform bool useTexture;
uniform float shadowIntensity = 1.0;

void main()
{
    vec4 textureColor;
    if (useTexture)
        textureColor = texture(image, TexCoords);
    else
        textureColor = vec4(1.0);

    // Desaturate the color by taking the luminance value
    float luminance = dot(textureColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 desaturatedColor = vec3(luminance);

    // Blend between the desaturated color and the sprite color based on shadowIntensity
    vec3 finalColor = mix(desaturatedColor, spriteColor.rgb, shadowIntensity);

    // Blend with the sprite color
    color = vec4(finalColor, textureColor.a * spriteColor.a);
}