#version 330

in vec3 Color;

uniform vec3 ComplementaryColor = vec3(-1,-1,-1);

uniform bool IsComplement = true;

out vec4 Fragment;

void main()
{
	if (IsComplement) {
		Fragment = vec4(abs(1 - Color.r), abs(1 - Color.g), abs(1- Color.b), 1);
	
		if(ComplementaryColor.r != -1.0 && ComplementaryColor.g != -1.0 && ComplementaryColor.b != -1.0)
			Fragment = vec4(abs(ComplementaryColor.r - 1), abs(ComplementaryColor.g - 1), abs(ComplementaryColor.b - 1), 1);
	}
		
	else {
		Fragment = vec4(Color, 1);
	
		if(ComplementaryColor.r != -1.0 && ComplementaryColor.g != -1.0 && ComplementaryColor.b != -1.0)
			Fragment = vec4(ComplementaryColor, 1);
	}

	return;
}