#version 430 core

layout(binding = 1) uniform sampler2D u_normalMap;
layout(binding = 2) uniform sampler2D ReflectionText;

uniform mat3 u_normalMatrix;
uniform vec3 u_lightDirection;
uniform vec4 u_color;
uniform vec3 camPos;

in vec2 v_texCoord;
in vec3 WorldPos;
in vec4 vReflectCoordinates;

out vec4 fragColor;

float u_exposure = 0.5;

vec3 hdr (vec3 color, float exposure) 
{
	return 1.0 - exp(-color * exposure);
}
		
void main (void)
{
	vec3 normal = texture2D( u_normalMap, v_texCoord ).rgb;

	vec3 view = normalize( camPos - WorldPos );
			
	vec3 R = normalize( reflect( -view, normalize(vec3(0.0f, 1.0f, 0.0f)) ) );

	vec3 reflection = normalize( reflect( -u_lightDirection, normal ) );
	
	float specularFactor = pow( max( 0.0, dot( view, reflection ) ), 500.0 ) * 200.0;
		
	vec3 distortion = normal * vec3( 0.1, 0.0, 0.1 ) ;
	//vec3 reflectionColor = texture2DProj( ReflectionText, vReflectCoordinates.xyz + distortion ).xyz;
	//vec3 reflectionColor = vec3( 0.1f, 0.1f, 0.1f ); 
	//vec3 reflectionColor = vec3( texture(skybox, v_texCoord + distortion.xz) ) * 0.4f; 
	vec2 texf = vec2( vReflectCoordinates.x, vReflectCoordinates.y ) / vReflectCoordinates.w * 0.5 + 0.5 + distortion.xz;
    vec3 reflectionColor = vec3(texture( ReflectionText, texf )) * 0.3f;

	float distanceRatio = min( 1.0, log( 1.0 / length( camPos - WorldPos ) * 3000.0 + 1.0 ) );
	distanceRatio *= distanceRatio;
	distanceRatio = distanceRatio * 0.7 + 0.3;
			
	normal = ( distanceRatio * normal + vec3( 0.0, 1.0 - distanceRatio, 0.0 ) ) * 0.5;
	normal /= length( normal );
			
	float fresnel = pow( 1.0 - dot( normal, view ), 2.0 );
			
	float skyFactor = ( fresnel + 0.2 ) * 10.0;
	vec3 waterColor = ( 1.0 - fresnel ) * vec3( 0.004, 0.016, 0.047 );
			
	vec3 color = ( skyFactor + specularFactor + waterColor ) * reflectionColor + waterColor * 0.5 ;
	color = hdr( color, u_exposure );

	fragColor = vec4( color, 1.0 );
}