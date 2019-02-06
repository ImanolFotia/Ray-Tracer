#version 440 core

uniform sampler2D texture0;

in vec2 TexCoords;

out vec4 FragColor;

const float gamma = 2.2;

void main()
{

	vec3 hdrColor = texture(texture0, TexCoords).rgb;
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, 1.0);
}