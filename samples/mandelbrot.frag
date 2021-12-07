/**
 * An example of fractal computed and rendered in a fragment shader.
 *
 * Filippo Maggioli (maggioli.filippo@gmail.com,maggioli@di.uniroma1.it)
 * Sapienza - University of Rome, Department of Computer Science
 */
#version 440 core

#define RESOLUTION  64


in vec2 UV;
out vec4 FragColor;


dvec2 csquare(dvec2 z)
{
    return dvec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y);
}


void main()
{
    dvec2 c = dvec2(UV);
    c.x = c.x * 2.5 - 1.75;
    c.y = c.y * 2.5 - 1.25;
    dvec2 z = c;

    int EscTime = 0;
    for (int i = 0; i < RESOLUTION; ++i)
    {
        z = csquare(z) + c;
        if (dot(z, z) > 2)
        {
            EscTime = RESOLUTION - i;
            break;
        }
    }

    FragColor = vec4(1.0f) * float(EscTime) / float(RESOLUTION);
}