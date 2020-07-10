#version 410

uniform sampler2D depthMap;
uniform vec2 blurDir;

in vec2 coord;

const float filterRadius = 10.0f; // or 10.0f
const float blurScale = 0.1f;
const float blurDepthFalloff = 300.0f; // or 300.0f

void main() {
	float depth = texture(depthMap, coord).x;

	if (depth <= 0.0f) {
		gl_FragDepth = 0;
		return;
	}

	if (depth >= 1.0f) {
		gl_FragDepth = depth;
		return;
	}
	
	float sum = 0.0f;
	float wsum = 0.0f;
	
	for (float x = -filterRadius; x <= filterRadius; x += 1.0f) {
		float sampleDepth = texture(depthMap, coord + x * blurDir).x;

		if (sampleDepth >= 1.0f) continue;
		
		// Spatial
		float r = x * blurScale;
		float w = exp(-r*r);
		
		float r2 = (sampleDepth - depth) * blurDepthFalloff;
		float g = exp(-r2*r2);
		
		sum += sampleDepth * w * g;
		wsum += w * g;
	}

	if (wsum > 0.0f) {
		sum /= wsum;
	}

	gl_FragDepth = sum;
	
}