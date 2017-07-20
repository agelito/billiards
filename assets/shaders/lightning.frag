// lightning.frag
#version 330

uniform sampler2D main_texture;

uniform vec3 light_direction;
uniform vec4 light_color_intensity;

uniform float ambient_coefficient;
uniform float roughness;
uniform float shininess;

uniform vec3 camera_position;

in vec3 position;
in vec3 normal;
in vec2 uv;
in vec3 tangent;
in vec3 binormal;

in vec3 eye_direction;

out vec4 out_color;

vec3 blinn_phong(vec3 normal, vec3 eye_direction, vec3 light_direction,
		 vec3 light_color, float light_intensity, vec3 diffuse_color)
{
    vec3 result = vec3(0.0, 0.0, 0.0);

    float lambertian = max(0.0, dot(light_direction, normal));

    if(lambertian > 0.0)
    {
	vec3 reflection = reflect(-light_direction, normal);
	float specular = max(dot(reflection, eye_direction), 0.0);
	float intensity = pow(specular, shininess);

	result = ((lambertian * diffuse_color) +
		  (light_color * intensity)) * light_intensity;
    }

    return result;
}

vec3 ward_specular(vec3 normal, vec3 tangent, vec3 binormal,
		   vec3 eye_direction, vec3 light_direction, vec3 light_color)
{
    float ps = 0.8;

    float ax = 0.5;
    float ay = 0.13;

    const float PI = 3.14159;

    float ndl = dot(normal, light_direction);

    float specular = 0.0;
    if(ndl > 0.0)
    {
	vec3 half_way = normalize(light_direction + eye_direction);
	float ndv = dot(normal, eye_direction);
	float hdn = dot(half_way, normal);
	float vdn = dot(eye_direction, normal);

	float hdt = dot(half_way, tangent) / ax;
	float hdb = dot(half_way, binormal) / ay;

	float exponent = exp(-2.0 * (hdt * hdt + hdb * hdb) / (1.0 + hdn));
	specular = ps * sqrt(max(0.0, ndl / ndv)) * exponent;
    }
    
    return light_color * specular;
}

vec3 oren_nayar(vec3 normal, vec3 eye_direction, vec3 light_direction, vec3 light_color)
{
    const float PI = 3.14159;
    
    float ndl = dot(normal, light_direction);
    float ndv = dot(normal, eye_direction);

    float angle_nv = acos(ndv);
    float angle_nl = acos(ndl);

    float alpha = max(angle_nv, angle_nl);
    float beta = min(angle_nv, angle_nl);

    float vdn = dot(eye_direction, normal);
    float ldn = dot(light_direction, normal);

    
    float gamma = dot(eye_direction - normal * vdn,
		      light_direction - normal * ldn);

    float roughness2 = (roughness * roughness);
    float roughness2_9 = (roughness2 / (roughness2 + 0.09));

    float c1 = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));
    float c2 = 0.45 * roughness2_9;

    if(gamma >= 0.0)
    {
	c2 *= sin(alpha);
    }
    else
    {
	c2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }

    float power = (4.0 * alpha * beta) / (PI * PI);
    float c3 = 0.125 * roughness2_9 * power * power;

    float a = gamma * c2 * tan(beta);
    float b = (1.0 - abs(gamma)) * c3 * tan((alpha + beta) / 2.0);

    float l1 = max(0.0, ndl) * (c1 + a + b);

    float b2pi = 2.0 * beta / PI;

    float l2 = 0.17 * max(0.0, ndl) * (roughness2 / (roughness2 + 0.13)) *
	(1.0 - gamma * b2pi * b2pi);

    vec3 final = light_color * (l1 + l2);
    
    return final;
}

void main()
{
    vec3 albedo = texture2D(main_texture, uv).rgb;

    vec3 gamma_to_linear = vec3(2.2);
    vec3 linear_albedo = pow(albedo, gamma_to_linear);
    
    vec3 ambient = (linear_albedo * ambient_coefficient);

    vec3 eye_direction = camera_position - position;

    vec3 n  = normalize(normal);
    vec3 ld = normalize(light_direction);
    vec3 ed = normalize(eye_direction);
    
    // vec3 on = oren_nayar(n, ed, ld, light_color_intensity.xyz);
    // vec3 lit_color = linear_albedo * on * light_color_intensity.w;

    vec3 t = normalize(tangent);
    vec3 b = normalize(binormal);
    
    // vec3 specular = ward_specular(n, t, b, ed, ld, light_color_intensity.xyz);
    vec3 lit_color = blinn_phong(n, ed, ld, light_color_intensity.xyz,
				 light_color_intensity.w, linear_albedo);

    vec3 gamma = vec3(1.0 / 2.2);
    vec3 linear_color = ambient + lit_color;
    vec3 output_color = pow(linear_color, gamma);
    
    out_color = vec4(output_color, 1.0);
}
