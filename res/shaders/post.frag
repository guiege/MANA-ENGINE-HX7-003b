#version 330 core
in  vec2  TexCoords;
out vec4  color;
  
uniform sampler2D scene;
uniform sampler2D mask;

uniform vec2 resolution;
uniform vec2 lightPos;
// The RGB values are the ambient light color
// and the alpha is the ambient intensity
uniform vec4 ambientData;
// The RGB values are the light color
// and the alpha is the light intensity
uniform vec4 lightData;
// Maximum radius of the light
uniform vec2 lightSize;



uniform vec2      offsets[9];
uniform int       edge_kernel[9];
uniform float     blur_kernel[9];
uniform vec2 rOffset;
uniform vec2 gOffset;
uniform vec2 bOffset;

uniform float fadeFactor;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform bool chromatic_aberration;
uniform bool lightsOut;

void main()
{
        // vec4 rValue = texture(scene, TexCoords - rOffset);  
        // vec4 gValue = texture(scene, TexCoords - gOffset);
        // vec4 bValue = texture(scene, TexCoords - bOffset);  

         vec4 sceneColor = texture(scene, TexCoords);
         vec4 fadeColor = vec4(0.0, 0.0, 0.0, 1.0);
         color = mix(fadeColor, sceneColor, fadeFactor);
    // color = vec4(0.0f);
    // vec3 sample[9];
    // // sample from texture offsets if using convolution matrix
    // if(chaos || shake)
    //     for(int i = 0; i < 9; i++)
    //         sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // // process effects

    // if(lightsOut)
    // {
    //     vec2 invertedLightPos = vec2(lightPos.x, resolution.y - lightPos.y);

    //     vec2 position = invertedLightPos / resolution.xx;

    //     float maxDistance = lightSize.x;

    //     // Gets the distance from the light's position and the fragment coord
    //     float distance = distance(gl_FragCoord.xy/resolution.xx, position);

    //     // Calculates the amount of light for the fragment
    //     float value = 1.0 - smoothstep(-0.2, maxDistance, distance);

    //     // Gets the original color from the texture
    //     vec4 pixel = texture2D(scene, TexCoords);

    //     float maskValue = texture2D(mask, TexCoords).r;

    //     // Applies the ambient light to the original pixel color
    //     vec3 ambient = pixel.rgb * ambientData.rgb * ambientData.a;

    //     // Calculates the light color for the pixel
    //     vec3 light = lightData.rgb * lightData.a * clamp(value, 0.0, 1.0) * maskValue;

    //     vec3 intensity = ambient + light;
    //     vec3 final = pixel.rgb * intensity;
    //     color = vec4(final, 1.0f);
    // }
    // else if (chaos)
    // {           
    //     for(int i = 0; i < 9; i++)
    //         color += vec4(sample[i] * edge_kernel[i], 0.0f);
    //     color.a = 1.0f;
    // }
    // else if (confuse)
    // {
    //     color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    // }
    // else if (shake)
    // {
    //     for(int i = 0; i < 9; i++)
    //         color += vec4(sample[i] * blur_kernel[i], 0.0f);
    //     color.a = 1.0f;
    // }
    // else if (chromatic_aberration)
    // {

    //     vec4 rValue = texture(scene, TexCoords - rOffset);  
    //     vec4 gValue = texture(scene, TexCoords - gOffset);
    //     vec4 bValue = texture(scene, TexCoords - bOffset);  

    //     color = vec4(rValue.r, gValue.g, bValue.b, 1.0);
    // }
    // else
    // {
    //     vec4 sceneColor = texture(scene, TexCoords);
    //     vec4 fadeColor = vec4(0.0, 0.0, 0.0, 1.0);
    //     color = mix(fadeColor, sceneColor, fadeFactor);
    // }
}