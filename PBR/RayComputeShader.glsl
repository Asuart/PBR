#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

struct Material {
    vec3 emissive ;  
    vec3 baseColor;
    float subsurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearcoat;
    float clearcoatGloss;
    float IOR;
    float transmission;
};

struct Triangle {
    vec3 p1, p2, p3;    
    vec3 n1, n2, n3;    
    Material material;  
};

struct EncodedTriangle {
	vec4 p1, p2, p3;    // Vertex coordinates
    vec4 n1, n2, n3;    // Vertex normal
    vec4 emissive;      // Self luminous parameters
    vec4 baseColor;     // colour
    vec4 param1;        // (subsurface, metallic, specular)
    vec4 param2;        // (specularTint, roughness, anisotropic)
    vec4 param3;        // (sheen, sheenTint, clearcoat)
    vec4 param4;        // (clearcoatGloss, IOR, transmission)
};

struct Ray {
    vec3 startPoint;
    vec3 direction;
};

struct HitResult {
    bool isHit;             
    bool isInside;          
    float distance;         
    vec3 hitPoint;          
    vec3 normal;           
    vec3 viewDir;           
    Material material;      
};

struct HitResultAligned {
    vec4 p;
    vec4 n;
};




#define SIZE_TRIANGLE   36
#define INF             114514.0



uniform int TRIANGLES_COUNT;
uniform int PATCH_SIZE;
uniform int PATCH_INDEX;

layout(std430, binding = 3) buffer TrianglesBuffer {
	EncodedTriangle triangles [];
};

layout(std430, binding = 4) buffer HitResultsBuffer {
	HitResultAligned hitResults [];
};



HitResult hitTriangle(EncodedTriangle triangle, Ray ray, float tMin, float tMax) {
    HitResult res;
    res.distance = INF;
    res.isHit = false;
    res.isInside = false;

    vec3 p1 = triangle.p1.xyz;
    vec3 p2 = triangle.p2.xyz;
    vec3 p3 = triangle.p3.xyz;

    vec3 S = ray.startPoint;   
    vec3 d = ray.direction;    
    vec3 N = normalize(cross(p2-p1, p3-p1));    

	float NdotD = dot(N, d);

    if (NdotD > 0.0f) {
        N = -N;   
        res.isInside = true;
    }

    if (abs(NdotD) < 0.00001f) return res;

    float t = (dot(N, p1) - dot(S, N)) / NdotD;
    if (t < tMin || t > tMax) return res;

    vec3 P = S + d * t;

    vec3 c1 = cross(p2 - p1, P - p1);
    vec3 c2 = cross(p3 - p2, P - p2);
    vec3 c3 = cross(p1 - p3, P - p3);
    bool r1 = (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0);
    bool r2 = (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0);

    if (r1 || r2) {
        res.isHit = true;
        res.hitPoint = P;
        res.distance = t;
        res.normal = N;
        res.viewDir = d;

        float alpha = (-(P.x-p2.x)*(p3.y-p2.y) + (P.y-p2.y)*(p3.x-p2.x)) / (-(p1.x-p2.x-0.00005)*(p3.y-p2.y+0.00005) + (p1.y-p2.y+0.00005)*(p3.x-p2.x+0.00005));
        float beta  = (-(P.x-p3.x)*(p1.y-p3.y) + (P.y-p3.y)*(p1.x-p3.x)) / (-(p2.x-p3.x-0.00005)*(p1.y-p3.y+0.00005) + (p2.y-p3.y+0.00005)*(p1.x-p3.x+0.00005));
        float gama  = 1.0 - alpha - beta;
        vec3 Nsmooth = alpha * triangle.n1.xyz + beta * triangle.n2.xyz + gama * triangle.n3.xyz;
        res.normal = (res.isInside) ? (-Nsmooth) : (Nsmooth);
    }

    return res;
}


HitResult TraceRay(Ray ray, float tMin, float tMax) {
    HitResult res;
    res.isHit = false;
    res.distance = INF;

    int START = PATCH_INDEX * PATCH_SIZE;
    int END = (PATCH_INDEX + 1) * PATCH_SIZE;
    if(TRIANGLES_COUNT < END) END = TRIANGLES_COUNT;

    for(int i = START; i < END; i++) {
		EncodedTriangle tri = triangles[i];
        HitResult r = hitTriangle(tri, ray, tMin, tMax);
        if(r.isHit && r.distance < res.distance) {
            res = r;
        }
    }

    return res;
}



void main() {

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);


	float max_x = 5.0;
	float max_y = 5.0;
	ivec2 dims = imageSize(img_output); // fetch image dimensions
	float x = (float(pixel_coords.x * 2 - dims.x) / dims.x);
	float y = (float(pixel_coords.y * 2 - dims.y) / dims.y);
	//vec3 ray_o = vec3(x * max_x, y * max_y, -4.0);
	//vec3 ray_d = vec3(0.0, 0.0, 1.0); // ortho

    vec3 ray_o = vec3(0, 0, -8);
	vec3 ray_d = vec3(x, y, 1.0); // ortho


	Ray ray;
	ray.startPoint = ray_o;
	vec3 dir = ray_d;
	ray.direction = normalize(dir);

    if(PATCH_INDEX == 0){
        hitResults[pixel_coords.y * dims.x + pixel_coords.x].p = vec4(0, 0, 0, INF);
        hitResults[pixel_coords.y * dims.x + pixel_coords.x].n = vec4(0, 0, 0, 0);
    }

	HitResult res = TraceRay(ray, 0.01, hitResults[pixel_coords.y * dims.x + pixel_coords.x].p.w);
	if(res.isHit) {

        vec3 color = vec3(1.0);
        for(int i = 0; i < 5; i++) {
            ray.startPoint = res.hitPoint;
            ray.direction = reflect(ray.direction, res.normal);
        }

        hitResults[pixel_coords.y * dims.x + pixel_coords.x].p = vec4(res.hitPoint, res.distance);
        imageStore(img_output, pixel_coords, vec4(color, 1));
    }
}