#version 410 core

uniform mat4 view;
uniform mat4 projection;

in vec4 gsColor;

out vec4 FragColor;

void main()
{
    const vec3 lightDir = vec3(0.577, 0.577, 0.577);
	//const vec2 texcoord = vec2(0.0, 0.0);
    // calculate normal from texture coordinates
    //vec3 N;
    //N.xy = texcoord * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    //float mag = dot(N.xy, N.xy);
    //if (mag > 1.0) discard;   // kill pixels outside circle
    //N.z = sqrt(1.0-mag);

    // calculate lighting
    //float diffuse = max(0.0, dot(lightDir, N));

    //FragColor = gsColor * diffuse;
	FragColor = gsColor;
}