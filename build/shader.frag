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
    Octree octarr[];
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
    return normalize(sign(centerToPoint) * step(-0.0025, abs(centerToPoint) - halfSize));
}

bool isPointInParallelepiped(vec3 point, vec3 minBounds, vec3 maxBounds) {
    return (point.x >= minBounds.x && point.x <= maxBounds.x &&
            point.y >= minBounds.y && point.y <= maxBounds.y &&
            point.z >= minBounds.z && point.z <= maxBounds.z);
}


void renderOctree(int rootInd, vec3 pos, vec3 ro, vec3 rd, out vec3 color){
    color = vec3(0.5, 0.5, 1.);
    int stInd[32];
    vec3 stPos[32];
    int stDepth[32];
    int stpr = 0;
    float minDists[16];
    for(int i = 0; i < 16; i++)
        minDists[i] = 99999.0;
    stInd[stpr] = rootInd;
    stPos[stpr] = pos;
    stDepth[stpr++] = 0;
    while(stpr > 0){
        rootInd = stInd[--stpr];
        pos = stPos[stpr];
        int depth = stDepth[stpr];
        vec3 size = vec3(5.0 / pow(2, depth));
        if(octarr[rootInd].isIntact == 1){
            if(octarr[rootInd].isColored == 1){
                vec3 inter = box(ro, rd, pos, pos + size);
                float c = dot(aabbNormal(pos, pos + size, inter.z * rd + ro), normalize(vec3(-1., -1., -1.)));
                if(abs(inter.z - minDists[depth]) > 0.001) continue;
                color = max(vec3(c), vec3(0.1));
                color *= abs(pos-vec3(0.0)) / 5.0;
                return;
                //if(abs(inter.z - minDists[depth]) <= 0.001) return;
                //if(inter.z != minDists[depth]) continue;
            }
            continue;
        }
        size /= 2.0;
        for(int i = 0; i < 8; i++){
            vec3 cpos = pos + (positions[i] * size.x);
            vec3 inter = box(ro, rd, cpos, cpos + size);
            if(bool(inter.x)){
                stInd[stpr] = octarr[rootInd].children[i];
                stPos[stpr] = cpos;
                stDepth[stpr] = depth + 1;
                minDists[depth + 1] = min(inter.z, minDists[depth + 1]);
                stpr++;
            }
        }
    }
}


void main() {
    vec2 uv = (gl_FragCoord.xy - .5 * res) / res.y;
    vec3 rd = normalize(vec3(uv, 1.0));
    rd.yz *= -mrot(angles.y);
    rd.xz *= -mrot(angles.x);
    vec3 ro = position;
    vec3 color = vec3(1);
    renderOctree(0, vec3(0), ro, rd, color);
    fragColor = vec4(color, 1.0);
}