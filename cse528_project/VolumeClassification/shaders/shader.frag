#version 330 core

in vec3 frag_texcoord;

out vec4 frag_color;

uniform sampler3D tex3D;
uniform vec3 eye;

bool outOfBox(vec3 voxel)
{
	float x = voxel.x;
	float y = voxel.y;
	float z = voxel.z;
	float max = x, min = x;

	max = max > y ? max : y;
	max = max > z ? max : z;
	min = min < y ? min : y;
	min = min < z ? min : z;

	// notice that voxel is texcoord [0,1]
	return (max > 1 || min < 0);
}

void main() {
	vec3 voxel;
	vec4 vcolor;
	vec3 dir = normalize(frag_texcoord - 0.5 - eye);
	vec3 step = dir * 0.01;
	vec4 accColor = vec4(0, 0, 0, 0);
	int numOfSteps = 0;

	for (int i = 0; i < 300; i++)
	{
		voxel = frag_texcoord + step * i;
		vcolor = texture(tex3D, voxel);

		if (accColor.a >= 1.0 || outOfBox(voxel))
		{
			accColor.a = 1.0f;
			break;
		} else
		{
			accColor.rgb = mix(accColor.rgb, vcolor.rgb, vcolor.a);
			accColor.a = mix(vcolor.a, 1.0, accColor.a);
		}
	}

	//frag_color = vec4(dir, 1);
	frag_color = accColor;
}


