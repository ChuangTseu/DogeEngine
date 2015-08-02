#version 400 core


vec2 screenTexCoord;

vec3 V;

vec3 B;
vec3 T;
vec3 N;

const int numLights = 1;

uniform bool diffuse;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 texcoordColor;

#define approx(val, target, delta) ( (val <= (target + delta) ) && (val >= (target - delta) ) )

bool vec_approx(vec3 v, vec3 target, float delta) {
    return (approx(v.x, target.x, delta) && approx(v.y, target.y, delta) && approx(v.z, target.z, delta));
}


vec3 toTanSpace(vec3 v) {
    return normalize(vec3(dot(v, T), dot(v, B), dot(v, N)));
}

vec3 fromTanSpace(vec3 v) {
    return normalize(mat3(T, B, N) * v);
}

vec3 fresnel_schlick(vec3 f0, float LdotH) {
    return f0 + (1 - f0) * pow(1 - LdotH, 5);
}


/* LEADR SPECIFIC */

// 2 GL_RGBA16F textures (becomes float vec4 here)
uniform sampler2D leadr1; //~x, ~y, ~x*~y, ~x²
uniform sampler2D leadr2; //~y², h

uniform sampler2D dispMapSampler;

float E_xn = 0;
float E_yn = 0;
float E_xnyn = 0;
float E_xn2 = 0;

float E_yn2 = 0;

float disp = 0;


float var_x = 0;
float var_y = 0;
float c_xy = 0;

vec4 stats1;
vec4 stats2;

//vec4 mean1 = textureLod(leadr1, screenTexCoord, 4096); // 4096 should be clamped to the max mip level
//vec4 mean2 = textureLod(leadr2, screenTexCoord, 4096); // giving us then the total mean

//END JUNK

/* Traitement LEADR */
void computeLeadrStatistics()
{
    float fakeLod = 0;

    stats1 = textureLod(leadr1, screenTexCoord, fakeLod);
    stats2 = textureLod(leadr2, screenTexCoord, fakeLod);

//    stats1 = texture(leadr1, screenTexCoord);
//    stats2 = texture(leadr2, screenTexCoord);

    float baseRoughnessOffset = 0;

    E_xn    = stats1.x;
    E_yn    = stats1.y;
    E_xnyn  = stats1.z;
    E_xn2   = stats1.w;

    E_yn2   = stats2.x;

    disp    = stats2.y;

    var_x = E_xn2 - E_xn*E_xn  + baseRoughnessOffset;

    var_y = E_yn2 - E_yn*E_yn  + baseRoughnessOffset;

    c_xy = E_xnyn - E_xn*E_yn;

}

vec3 getMesoTan() {
    return normalize(vec3(-E_xn, -E_yn, 1));
}

vec3 getMesoWorld() {
    return normalize(-E_xn*T - E_yn*B + N);
}

float P22(float x, float y)
{
    x -= E_xn;
    y -= E_yn;

    float det = var_x * var_y - c_xy * c_xy ;

//    return exp(88.73);

    if (approx(det, 0, 0.0000000000001)) // "Dirac if plane"
    {
//        return float((x == 0) && (y == 0));

        return float(approx(x, 0, 0.0001) && approx(y, 0, 0.0001));
    }

//    return det + 0.1;

    float arg_exp = -0.5 * ( x * x * var_y + y * y * var_x - 2.0* x * y * c_xy ) / det ;

//    return arg_exp;

//    float val = arg_exp;

//    return exp( arg_exp - 3000000000000000000.0);

    float P22_ = 0.15915 * inversesqrt ( (det) ) * exp ( arg_exp );

    return float(P22_ > 1);
}

float D_beckmann(vec3 n) {
    float x = -n.x / n.z;
    float y = -n.y / n.z;

    float P22_ = P22(x, y);

    float D_ = P22_ / (n.z * n.z * n.z * n.z); // Here, n.z is the same as dot(wn, wg) given in the formula

//    return (approx(n.z, dot(n, toTanSpace(N)), 0) ? 1 : 0); WORKS ! PROOF

//    return float(isnan(P22_) || isinf(P22_));

//    return P22_;

    return D_;
}

float D_dirac(vec3 n) {
    return (vec_approx(n, vec3(0, 0, 1), 0.01) ? 1 : 0);
//    return (n == vec3(0, 0, 1) ? 1 : 0);
}

float D_cos(vec3 n) {
    vec3 mesoTan = getMesoTan();

    return max(0.0, dot(n, mesoTan));
}

float smith(vec3 v) {
    vec2 v_xy = normalize(vec2(dot(v, T), dot(v, B)));

    float mu_phi = v_xy.x * E_xn + v_xy.y * E_yn;
    float var_phi = v_xy.x * v_xy.x * var_x + v_xy.y * v_xy.y * var_y + 2.0* v_xy.x * v_xy.y * c_xy ;

    float cos_theta_v = dot(v , N);

    float cot_theta_v = cos_theta_v / sqrt(1.0 - cos_theta_v * cos_theta_v);

    float nu_v = clamp((cot_theta_v - mu_phi) * inversesqrt(2.0 * var_phi), 0.001, 1.599);

    float Lambda_ = (1.0 -1.259 * nu_v + 0.396 * nu_v * nu_v) / (3.535 * nu_v + 2.181 * nu_v * nu_v);

    return Lambda_ ;
}


float roughSpecularPointLight(vec3 L) {

    vec3 H = normalize(L + V);

    vec3 Htan = toTanSpace(H);
    vec3 Ltan = toTanSpace(L);
    if ( Htan.z <= 0.0) return 0.0;

    float d = D_beckmann(Htan);

    float lambda_V = smith((V)) * 1;
    float lambda_L = smith((L)) * 1;

    float masking_shadowing_terms = 1 / (1 + lambda_V + lambda_V);

    float inverse_projected_area = 1 / dot(getMesoWorld(), V);

//    float I = 0.25 * inverse_projected_area * d * masking_shadowing_terms;

    float I = d;

    return I;


    return I + 10000;
}


/* LEADR SPECIFIC END */

vec3 dumbTest(vec3 self) {
//    return fromTanSpace((getMesoTan()));

//    return (E_yn < 0 ? vec3(E_yn) : vec3(1, 0, 0));

//    return vec3(float(c_xy > 10));

//    return texture(dispMapSampler, screenTexCoord).xyz;

    if (diffuse)
        return vec3(stats1.x);
    else
        return vec3(stats2.x);

    return vec3(P22(0, 0));

    return vec3(D_beckmann(vec3(0.0, 0.0, 1.0)));

    return self;
}

uniform int screenWidth;
uniform int screenHeight;



void main( void )
{
    screenTexCoord = vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight);

    V = vec3(1, 0, 0);


    computeLeadrStatistics();

    vec3 finalColor;

    vec3 worldNormal = normalize(vec3(1,0,0));
    vec3 worldTangent = normalize(vec3(0,0,-1));
    vec3 worldBitangent = normalize(cross(worldNormal, worldTangent));

    N = worldNormal;
    T = worldTangent;
    B = worldBitangent;

    normalColor = vec4(getMesoTan(), 1);
    texcoordColor = vec4(screenTexCoord, 0, 1);

    vec3 pointL = normalize(vec3(1, 0, 0));

    vec3 pointH = normalize(pointL + V);

    vec3 H = normalize(pointL + V);

    vec3 Htan = toTanSpace(H);

//    fragColor.xyz = vec3(roughSpecularPointLight(pointL)); // WARNING ! NaN detected here :(

//    fragColor.xyz = roughSpecularCubeMap();

    vec3 mesoN = normalize(-E_xn*T - E_yn*B + N);

//    fragColor.xyz = blinn_phong_calc(pointLight, mesoN);

//    fragColor.xyz = texture(leadr2, screenTexCoord).yyy;

//    float val = textureLod(leadr2, screenTexCoord, 4096).y;

//    fragColor.xyz = vec3(D_beckmann(toTanSpace(V)));

//    fragColor.xyz = texture(dispMapSampler, screenTexCoord).yyy;

//    fragColor.xyz = vec3(V.z);

//    normal = normalize(inData.normal);

//    algo2();

//    float albedo = 6.5;

//    fragColor.xyz = E(N) * albedo;

//    fragColor.xyz = vec3(D_beckmann(normalize(vec3(1, 1, 1))));

//    fragColor.xyz = texture(dispMapSampler, screenTexCoord).xyz;

    fragColor.xyz = dumbTest(fragColor.xyz);
}
