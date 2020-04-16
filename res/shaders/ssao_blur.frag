#version 430 core

layout(binding = 0) uniform sampler2D normalDepthDiffuseSampler;
layout(binding = 1) uniform sampler2D ssaoSampler;

uniform layout(location = 0) int kernel_size;
uniform layout(location = 1) vec2 pixel_size;
uniform layout(location = 2) vec2 sample_dir;

in layout(location = 0) vec4 pos_in;

out vec4 color;

// Makes sure that the filter does not blur sharp edges
bool cutoffTest(vec3 normal1, vec3 normal2)
{
	return dot(normal1, normal2) > 0.95;
}

void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 center_normal = vec3(texture(normalDepthDiffuseSampler, uv).xy, 0.0);
	center_normal.z = sqrt(1 - length(center_normal) * length(center_normal));

	float result = texture(ssaoSampler, uv).x;
	float acc = 1.0f;

	for (int i = 1; i <= kernel_size / 2; i++)
	{
		vec2 sample_pos = uv + float(i) * sample_dir * pixel_size;
		vec3 sample_normal = vec3(texture(normalDepthDiffuseSampler, sample_pos).xy, 0.0);
		sample_normal.z = sqrt(1 - length(sample_normal) * length(sample_normal));
		if (cutoffTest(center_normal, sample_normal))
		{
			result += texture(ssaoSampler, sample_pos).x;
			acc += 1.0f;
		}
	}

	for (int i = 1; i <= kernel_size / 2; i++)
	{
		vec2 sample_pos = uv - float(i) * sample_dir * pixel_size;
		vec3 sample_normal = vec3(texture(normalDepthDiffuseSampler, sample_pos).xy, 0.0);
		sample_normal.z = sqrt(1 - length(sample_normal) * length(sample_normal));
		if (cutoffTest(center_normal, sample_normal))
		{
			result += texture(ssaoSampler, sample_pos).x;
			acc += 1.0f;
		}
	}


    color = vec4( (result / acc).xxx, 0.0);
}