/**
 * An example of gradient with gamma correction.
 *
 * Filippo Maggioli (maggioli.filippo@gmail.com,maggioli@di.uniroma1.it)
 * Sapienza - University of Rome, Department of Computer Science
 */
#version 440 core

in vec2 UV;
out vec4 FragColor;

uniform float Time;

void main()
{
    // FragColor = vec4(UV, sin(Time) / 2.0f + 0.5f, 1.0f);
    FragColor = vec4(sqrt(UV.x), sqrt(UV.y), sin(Time) / 2.0f + 0.5f, 1.0f);
}