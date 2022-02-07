#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;


uniform vec4 color;
uniform float thickness;

void main() {
    vec4 texelColor = texture2D(texture0, fragTexCoord);
	
	float _thickness = ( thickness / 256.0);

	if(texelColor.a > 0.0) {
		gl_FragColor = texelColor * colDiffuse;
	}
	else {
		float left = texture2D(texture0, vec2(fragTexCoord.x + _thickness, fragTexCoord.y)).a;
		float right = texture2D(texture0, vec2(fragTexCoord.x - _thickness, fragTexCoord.y)).a;
		float top = texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y + _thickness)).a;
		float bottom = texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y - _thickness)).a;
		
		if( texelColor.a < 1.0 ) {
			if( left > 0.0 || right > 0.0 || top > 0.0 || bottom > 0.0 ) {
				gl_FragColor = color  * colDiffuse;
			}
		}
		else {
			gl_FragColor = texelColor * colDiffuse;
		}
	}
}