#version 440 core

in vec2 UV;
out vec4 FragColor;

void main()
{
    // FragColor = vec4(UV, 1.0f, 1.0f);
    FragColor = vec4(sqrt(UV.x), sqrt(UV.y), 1.0f, 1.0f);
}