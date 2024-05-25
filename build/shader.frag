#version 430 core

struct Octree {
     int children[8];
     int isIntact;
     int isColored;
};

mat2 mrot(float a){
    float c = cos(a);
    float s = sin(a);
    return mat2(c,-s,s,c);
}

layout(std430, binding = 0) buffer OctreeBuffer {
    Octree octreeArr[];
};

uniform vec2 res;
uniform vec3 position;
uniform vec2 angles;

out vec4 fragColor;
in vec3 vertexColor;

vec3 positions[8] = vec3[](
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 1.0, 1.0)
);

vec3 box(vec3 ray_origin, vec3 ray_dir, vec3 minpos, vec3 maxpos) {
    vec3 inverse_dir = 1.0 / ray_dir;
    vec3 tbot = inverse_dir * (minpos - ray_origin);
    vec3 ttop = inverse_dir * (maxpos - ray_origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 traverse = max(tmin.xx, tmin.yz);
    float traverselow = max(traverse.x, traverse.y);
    traverse = min(tmax.xx, tmax.yz);
    float traversehi = min(traverse.x, traverse.y);
    return vec3(float(traversehi > max(traverselow, 0.0)), traversehi, traverselow);
}

vec3 aabbNormal(const vec3 bmin, const vec3 bmax, const vec3 point) {
    const vec3 center = 0.5 * (bmin + bmax);
    const vec3 centerToPoint = point - center;
    const vec3 halfSize = 0.5 * (bmax - bmin);
    return normalize(sign(centerToPoint) * step(-0.01, abs(centerToPoint) - halfSize));
}

bool isPointInParallelepiped(vec3 point, vec3 minBounds, vec3 maxBounds) {
    return (point.x >= minBounds.x && point.x <= maxBounds.x &&
            point.y >= minBounds.y && point.y <= maxBounds.y &&
            point.z >= minBounds.z && point.z <= maxBounds.z);
}


void renderOctree(int rootInd, vec3 pos, float depth, vec3 ro, vec3 rd, out vec3 color){
    color = vec3(0.2);
    vec3 oro = ro;
    vec3 lastHP = ro;
    for(int i = 0; i < 10; i++){
        while(true){
            vec3 size = vec3(5.0 / pow(2.0, depth));
            if(octreeArr[rootInd].isIntact == 1){
                if(octreeArr[rootInd].isColored == 1){
                    vec3 inter = box(oro, rd, pos, pos + size);
                    //float c = dot(aabbNormal(pos, pos + size, inter.z * rd + ro), normalize(vec3(1., 1., 0.)));
                    color = vec3(1.) - vec3(inter.z / 10.);
                    return;
                }
                ro = lastHP;
                break;
            }
            size /= 2.0;
            float minDist = 999999.0;
            vec3 npos = vec3(0);
            bool c = true;
            int nrootInd = 0;
            for(int i = 0; i < 8; i++){
                vec3 cpos = pos + (positions[i] * size.x);
                vec3 nro = ro + rd * 0.01;
                vec3 inter = box(nro, rd, cpos, cpos + size);
                int ind = octreeArr[rootInd].children[i];
                bool isContaining = isPointInParallelepiped(nro, cpos, cpos + size);
                if((inter.z < minDist && bool(inter.x)) || isContaining){
                    c = false;
                    lastHP = inter.y * rd + nro;
                    minDist = inter.z;
                    nrootInd = ind;
                    npos = cpos;
                    if(isContaining) break;
                }
            }
            if(c) break;
            rootInd = nrootInd;
            pos = npos;
            depth += 1.0;
        }
    }
}


void main(){
    vec2 uv = (gl_FragCoord.xy - .5 * res) / res.y;
    vec3 rd = normalize(vec3(uv, 1.0));
    rd.yz *= -mrot(angles.y);
    rd.xz *= -mrot(angles.x);
    vec3 ro = position;
    vec3 color = vec3(0);
    renderOctree(0, vec3(0), 0.0, ro, rd, color);
    fragColor = vec4(color, 1.0);
}