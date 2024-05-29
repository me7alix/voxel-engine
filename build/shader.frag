#version 430 core

struct Octree {
     int children[8];
     int isIntact;
     int isColored;
     int isCollapsed;
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

vec3 cubeNml(vec3 i, vec3 bmin, vec3 bmax) {
    float epsilon = 0.01;

    float cx = abs(i.x - bmin.x);
    float fx = abs(i.x - bmax.x);
    float cy = abs(i.y - bmin.y);
    float fy = abs(i.y - bmax.y);
    float cz = abs(i.z - bmin.z);
    float fz = abs(i.z - bmax.z);

    if(cx < epsilon)
        return vec3(-1.0, 0.0, 0.0);
    else if (fx < epsilon)
        return vec3(1.0, 0.0, 0.0);
    else if (cy < epsilon)
        return vec3(0.0, -1.0, 0.0);
    else if (fy < epsilon)
        return vec3(0.0, 1.0, 0.0);
    else if (cz < epsilon)
        return vec3(0.0, 0.0, -1.0);
    else if (fz < epsilon)
        return vec3(0.0, 0.0, 1.0);
    
    return vec3(0.0, 0.0, 0.0);
}

vec3 aabbNormal(const vec3 bmin, const vec3 bmax, const vec3 point) {
    const vec3 center = 0.5 * (bmin + bmax);
    const vec3 centerToPoint = point - center;
    const vec3 halfSize = 0.5 * (bmax - bmin);
    return normalize(sign(centerToPoint) * step(-0.005, abs(centerToPoint) - halfSize));
}

bool isPointInParallelepiped(vec3 point, vec3 minBounds, vec3 maxBounds) {
    return (point.x >= minBounds.x && point.x <= maxBounds.x &&
            point.y >= minBounds.y && point.y <= maxBounds.y &&
            point.z >= minBounds.z && point.z <= maxBounds.z);
}


void renderOctree(int rootInd, vec3 pos, float depth, vec3 ro, vec3 rd, out vec3 color){
    color = vec3(0.2);
    vec3 oro = ro;
    float maxHP = 0.0;
    vec3 lastHP = vec3(0);

    int oldRootInd = rootInd;
    vec3 oldPos = pos;
    for(int k = 0; k < 50; k++){
        while(true){
            vec3 size = vec3(5.0 / pow(2.0, depth));
            if(octarr[rootInd].isIntact == 1){
                if(octarr[rootInd].isColored == 1){
                    vec3 inter = box(oro, rd, pos, pos + size);
                    float c = dot(aabbNormal(pos, pos + size, inter.z * rd + oro), normalize(vec3(-1., -1., -1.)));
                    //float c = dot(cubeNml(oro + rd * inter.z, pos, pos + size), normalize(vec3(-1., 1., 1.)));
                    //color = vec3(1.) - vec3(inter.z / 10.);
                    color = max(vec3(c), vec3(0.1));
                    color *= abs(pos-vec3(2.5)) / 5.0;
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
                vec3 inter = box(ro, rd, cpos, cpos + size);
                vec3 nro = ro + rd * 0.001;
                int ind = octarr[rootInd].children[i];
                bool isContaining = isPointInParallelepiped(nro, cpos, cpos + size);
                if(isContaining) {
                    c = false;
                    inter = box(oro, rd, cpos, cpos + size);
                    lastHP = oro + rd * inter.y;
                    npos = cpos;
                    nrootInd = ind;
                    break;
                }
                if(inter.z < minDist && bool(inter.x)){
                    c = false;
                    lastHP = inter.y * rd + ro;
                    minDist = inter.z;
                    nrootInd = ind;
                    npos = cpos;
                }
            }
            if(c) return;
            rootInd = nrootInd;
            pos = npos;
            depth += 1.0;
        }
        pos = oldPos;
        rootInd = oldRootInd;
        depth = 0.0;
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