#version 430 core

#define NUM_SAMPLES 16

layout(binding = 0) uniform sampler2D normalDepthDiffuseSampler;
layout(binding = 1) uniform sampler2D randomSampler;

uniform layout(location = 0) mat4 projection;
uniform layout(location = 1) mat4 inv_projection;
uniform layout(location = 2) vec2 randomScale;
uniform layout(location = 3) float radius;
uniform layout(location = 4) vec3 samples[NUM_SAMPLES];

in layout(location = 0) vec4 pos_in;
in layout(location = 1) vec4 view_pos_in;

out vec4 color;

void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 normalDepth = texture(normalDepthDiffuseSampler, uv).xyz;
    vec3 normal = vec3(normalDepth.xy, sqrt(1 - length(normalDepth.xy) * length(normalDepth.xy)));
    float depth = normalDepth.z;
    vec3 fragPosVS = vec3(view_pos_in.xy, 1.0) * depth;
    vec3 randomDir = texture(randomSampler, uv * randomScale).xyz;

    vec3 tangent = normalize(randomDir - normal * dot(randomDir, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_SAMPLES; i++)
    {
        vec3 samplePosVS = fragPosVS + tbn * samples[i] * radius;

        // Project samplepos to clip space
        vec4 samplePosCS = projection * vec4(samplePosVS, 1.0f);
        samplePosCS.xy /= samplePosCS.w;
        samplePosCS.xy = samplePosCS.xy * 0.5 + 0.5;

        float sampleDepth = texture(normalDepthDiffuseSampler, samplePosCS.xy).z;

        float a = 0.0;

        // Dont count points that are outside the radius
        a = abs(fragPosVS.z - sampleDepth) < radius ? 1 : 0;
        
        // Point is occluded if the sample-depth is larger than the samplepoints depth
        a *= (sampleDepth >= samplePosVS.z) ? 1 : 0;

        occlusion += a;
    }

    color = vec4( 1.0 - occlusion / NUM_SAMPLES, 0.0, 0.0, 0.0);
}