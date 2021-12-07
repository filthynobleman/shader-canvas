/**
 * A sample ray marching scene.
 *
 * Filippo Maggioli (maggioli.filippo@gmail.com,maggioli@di.uniroma1.it)
 * Sapienza - University of Rome, Department of Computer Science
 */
#version 440 core

#define MAX_STEPS       100
#define MAX_DIST        100.0f
#define SURF_HIT        0.001f
#define EPSILON         0.00001f

#define CAMERA_FOV      120.0f
#define CAMERA_POS      vec3(0.0f, 1.0f, 0.0f)

#define SPHERE_POS      vec3(0.0f, 1.0f, 6.0f)
#define SPHERE_RAD      1.0f

#define LIGHT_POS       vec3(2.0f * sin(Time), 3.0f, 4.0f + 2.0f * cos(Time))


float sphere_dist(vec3 P)
{
   return length(P - SPHERE_POS) - SPHERE_RAD;
}

float plane_dist(vec3 P)
{
    return P.y;
}

float scene_dist(vec3 P)
{
    return min(sphere_dist(P), plane_dist(P));
}


vec3 normal(vec3 P)
{
    float gx = scene_dist(P + vec3(EPSILON, 0.0f, 0.0f)) - scene_dist(P - vec3(EPSILON, 0.0f, 0.0f));
    float gy = scene_dist(P + vec3(0.0f, EPSILON, 0.0f)) - scene_dist(P - vec3(0.0f, EPSILON, 0.0f));
    float gz = scene_dist(P + vec3(0.0f, 0.0f, EPSILON)) - scene_dist(P - vec3(0.0f, 0.0f, EPSILON));

    return normalize(vec3(gx, gy, gz));
}


float ray_march(vec3 RayOrig, vec3 RayDir)
{
    float RayDist = 0.0f;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        if (RayDist >= MAX_DIST)
            break;

        vec3 RayPos = RayOrig + RayDist * RayDir;
        float StepDist = scene_dist(RayPos);
        RayDist += StepDist;
        if (StepDist < SURF_HIT)
            break;
    }

    return RayDist;
}



in vec2 UV;
out vec4 FragColor;

uniform float Time;

void main()
{
    vec3 RayOrig = CAMERA_POS;
    vec3 RayDir = vec3((2.0 * UV - 1.0f) * CAMERA_FOV / 180.0f, 1.0f);
    float RayDist = ray_march(RayOrig, RayDir);

    vec3 RayPos = RayOrig + RayDist * RayDir;
    vec3 Normal = normal(RayOrig + RayDist * RayDir);
    vec3 LightDir = LIGHT_POS - RayPos;
    float Diffuse = max(0.0f, dot(Normal, normalize(LightDir)));

    float LightObstacle = ray_march(RayPos + Normal * SURF_HIT * 1.5f, LightDir);
    Diffuse *= clamp(LightObstacle / length(LightDir), 0.0f, 1.0f);

    FragColor = vec4(1.0f) * Diffuse;
}