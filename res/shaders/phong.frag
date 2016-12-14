#version 440

// A directional light structure
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};

// A material structure
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 1) in vec3 normal;
// Incoming texture coordinate
layout (location = 2) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	// ***************************
	// Calculate ambient component
	// ***************************
	vec4 ambient_component = light.ambient_intensity * mat.diffuse_reflection;

	// ***************************
	// Calculate diffuse component
	// ***************************
	vec4 C = light.light_colour;
	vec4 D = mat.diffuse_reflection;

	vec4 diffuse_component = max(dot(normal, light.light_dir), 0) * (D*C);

	// ************************
	// Calculate view direction
	// ************************
	vec3 eye_direction = normalize(eye_pos - position);

	// *********************
	// Calculate half vector
	// *********************
	vec3 half_vector = normalize(light.light_dir + eye_direction);

	// ****************************
	// Calculate specular component
	// ****************************
	vec4 S = mat.specular_reflection;

	vec4 specular_component = pow(max(dot(normal, half_vector), 0), mat.shininess) * (S*C);

	// **************
	// Sample texture
	// **************
	vec4 tex_colour = texture(tex, tex_coord);

	// **********************************
	// Calculate primary colour component
	// **********************************
	vec4 primary = mat.emissive + ambient_component + diffuse_component;

	// **********************
	// Calculate final colour
	// - remember alpha 1.0
	// **********************
	colour = primary * tex_colour + specular_component;
	colour.a = 1.0;

}