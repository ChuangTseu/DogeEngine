#version 400 core

#define M_PI 3.14159265358979323846

uniform sampler2D texSampler;
uniform sampler2D normalMapSampler;

uniform sampler2D dispMapSampler;

uniform sampler2D sphericalMapSampler;

uniform mat4 lightMVP;

in Data {
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;

    vec3 position;
} inData;


struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform DirLight dirLight;
uniform PointLight pointLight;

uniform vec3 eyePosition;

//Material
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

//UI Communication Uniforms
uniform bool filtering;
uniform bool diffuse;
uniform bool specularDirect;
uniform bool specularEnv;

uniform bool diffuseDirect;
uniform bool diffuseEnv;

uniform int currentBRDF;

uniform float fresnel0;

uniform float roughnessOffset;

vec3 V;

vec3 B;
vec3 T;
vec3 N;

const int numLights = 1;

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

float fresnel_schlick(float f0, float LdotH) {
    return f0 + (1.0 - f0) * pow(1.0 - LdotH, 5.0);
}

float fresnel_nvidiagpugems3(float f0, float VdotH) {
    float base = 1.0 - VdotH;
    float exponential = pow(base, 5.0);

    return exponential + f0 * (1.0 - exponential);
}




// 2 GL_RGBA16F textures (becomes float vec4 here)
uniform sampler2D leadr1; //~x, ~y, ~x*~y, ~x²
uniform sampler2D leadr2; //~y², h

float E_xn = 0;
float E_yn = 0;
float E_xnyn = 0;
float E_xn2 = 0;

float E_yn2 = 0;

float disp = 0;

float var_x = 0;
float var_y = 0;
float c_xy = 0;



/* Traitement LEADR */
void computeLeadrStatistics()
{
    vec4 stats1;
    vec4 stats2;


    if (filtering) {
        stats1 = texture(leadr1, inData.texcoord);
        stats2 = texture(leadr2, inData.texcoord);
    }
    else {
        float fakeLod = 0;

        stats1 = textureLod(leadr1, inData.texcoord, fakeLod);
        stats2 = textureLod(leadr2, inData.texcoord, fakeLod);
    }



    float baseRoughnessOffset = roughnessOffset + 0.000001; // Try to ensure no zero divide or whatever

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

    float arg_exp = -0.5 * ( x * x * var_y + y * y * var_x - 2.0* x * y * c_xy ) / det ;

    float P22_ = 0.15915 * inversesqrt ( det ) * exp ( arg_exp );

//    return float(P22_ < 1.6);

    return P22_;
}

float D_beckmann(vec3 n) {
    float x = -n.x / n.z;
    float y = -n.y / n.z;


    float P22_ = P22(x, y);

    float D_ = P22_ / (n.z * n.z * n.z * n.z); // Here, n.z is the same as dot(wn, wg) given in the formula

//    return (approx(n.z, dot(n, toTanSpace(N)), 0) ? 1 : 0); // WORKS ! PROOF

//    return float(isnan(P22_) || isinf(P22_));

    return D_;
}

float D_beckmann_testbed(vec3 n) {
    float x = -n.x / n.z;
    float y = -n.y / n.z;


    x -= E_xn;
    y -= E_yn;

    float det = var_x * var_y - c_xy * c_xy ;

    float P22_ = 0;

//    if (det < 0.00001) // "Dirac if plane"
//    {
//        P22_ = 0;
//    }
//    else
//    {
        float arg_exp = -0.5 * ( x * x * var_y + y * y * var_x - 2.0* x * y * c_xy ) / det ;

        P22_ = 0.15915 * inversesqrt ( (det) ) * exp ( arg_exp );
//    }


    float D_ = P22_ / (n.z * n.z * n.z * n.z); // Here, n.z is the same as dot(wn, wg) given in the formula

    return D_;
}

float D_dirac(vec3 n) {
    return (vec_approx(n, vec3(0, 0, 1), 0.01) ? 1 : 0);
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

    float Lambda_ = (1.0 - 1.259 * nu_v + 0.396 * nu_v * nu_v) / (3.535 * nu_v + 2.181 * nu_v * nu_v);

    return Lambda_ ;
}


float roughSpecularPointLight(vec3 L) {

    vec3 H = normalize(L + V);

    vec3 Htan = toTanSpace(H);
    vec3 Ltan = toTanSpace(L);
    vec3 Vtan = toTanSpace(V);
    if ( Htan.z <= 0.0) return 0.0;

    float d = D_beckmann(Htan);

    return d;

    float lambda_V = smith(V) * 1;
    float lambda_L = smith(L) * 1;

    float masking_shadowing_terms = 1 / (1 + lambda_V + lambda_V);

    float inverse_projected_area = 1 / dot(getMesoWorld(), V);

    float I = 0.25 * inverse_projected_area * d * masking_shadowing_terms;

    return I;
}


const int nbSamples = 5;
const float sampleLength = 0.9;

const float p[5] = float[5](-1.8, -0.9, 0, 0.9, 1.8);
const float w[5] = float[5](0.0725, 0.2443, 0.3663, 0.2443, 0.0725);

vec3 fetchSphericalMap(vec3 v, float LOD) {
    float norm = inversesqrt(v.x*v.x + v.y*v.y + v.z*v.z);

    float DDx = v.x * norm;
    float DDy = v.y * norm;
    float DDz = v.z * norm;

    float r = 0.159154943 * acos(DDz) * inversesqrt(DDx*DDx + DDy*DDy);

    float sb_u = 0.5 + DDx * r;
    float sb_v = 0.5 + DDy * r;

    return textureLod(sphericalMapSampler, vec2(sb_u, sb_v), LOD).rgb;
}

/* TODO : see how to integrate smith terms in this equation */
vec3 roughSpecularCubeMap() {
    float sigma_x = sqrt(var_x);
    float sigma_y = sqrt(var_y);

    float r_xy = 1;

    if (sigma_x*sigma_y != 0)
        r_xy = c_xy / (sigma_x*sigma_y);

    float sq_one_minus_r_xy2 = sqrt(1 - r_xy*r_xy);

//    float LODoffset = -1.48 + log2(sampleLength*max(sigma_x, sigma_y)* float(textureSize(cubeMapSampler, 0).x));

    // Replace -1.48 by 0.5 as said in the supplemental
    float LODoffset = 0.5 + log2(sampleLength*max(sigma_x, sigma_y) * float(textureSize(sphericalMapSampler, 0).x));

    float S = 0;
    vec3 I = vec3(0);


    for (int j = 0; j < nbSamples; ++j) {
        for (int k = 0; k < nbSamples; ++k) {
            float x = p[j]*sigma_x + E_xn;
            float y = (r_xy*p[j] + sq_one_minus_r_xy2*p[k]) * sigma_y + E_yn;


            // microfacet normal
            vec3 meso = getMesoWorld();

            // microfacet projected area
            float dotVN = dot(V, meso);
            float dotZN = dot(N, meso);
            float S_ = dotVN / dotZN;

            // reflected vector
            vec3 R = reflect(-V, meso);

            // Jacobian : slope space area to solid angle
            float J = abs(4.0 * dotZN*dotZN*dotZN * dotVN);

            // LOD in cube map
            float LOD = 0.72*log(J) + LODoffset ;
            // radiance
//            vec3 I_ = textureLod(cubeMapSampler, R, LOD).rgb;
            vec3 I_ = fetchSphericalMap(R, LOD);
            // sum up
            S += w[j]*w[k] * S_ ;
            I += w[j]*w[k] * S_ * I_ ;
        }
    }

    return I / S;
}


uniform mat4 hred;
uniform mat4 hgreen;
uniform mat4 hblue;

uniform vec3 shc_env[9];

#define L00     0
#define L1_1    1
#define L10     2
#define L11     3
#define L2_2    4
#define L2_1    5
#define L20     6
#define L21     7
#define L22     8

vec3 Llm[9] = vec3[9](vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0), vec3(0));

/* Algo 2 */
void algo2(vec3 LD, vec3 LC)
{
    float smith_wo = smith(V) * 0.001;

    for (int i = 0; i < 9; ++i) {
        Llm[i] = vec3(0);
    }

//    vec3 LD = normalize(pointLight.position - eyePosition);
//    vec3 LC = pointLight.color;
    if (diffuseDirect) {
        for (int i = 0; i < numLights; ++i) {
            vec3 Li = LC; //vec3(0);
            vec3 wi = LD; //vec3(0);

            float xi = wi.x;
            float yi = wi.y;
            float zi = wi.z;

            float smith_wi = smith(wi);

            vec3 P = Li/(1.0 + smith_wo + smith_wi);

            float c;

            /* L_{00}.  Note that Y_{00} = 0.282095 */
            c = 0.282095 ;
            Llm[L00] += c * P;

            /* L_{1m}. -1 <= m <= 1.  The linear terms */
            c = 0.488603 ;
            Llm[L1_1] += c * yi * P;
            Llm[L10] += c * zi * P;
            Llm[L11] += c * xi * P;

            /* The Quadratic terms, L_{2m} -2 <= m <= 2 */

            /* First, L_{2-2}, L_{2-1}, L_{21} corresponding to xy,yz,xz */
            c = 1.092548 ;
            Llm[L2_2] += c * xi*yi * P;
            Llm[L2_1] += c * yi*zi * P;
            Llm[L21] += c * xi*zi * P;

            /* L_{20}.  Note that Y_{20} = 0.315392 (3z^2 - 1) */
            c = 0.315392 ;
            Llm[L20] += c * (3*zi*zi - 1.0) * P;

            /* L_{22}.  Note that Y_{22} = 0.546274 (x^2 - y^2) */
            c = 0.546274 ;
            Llm[L22] += c * (xi*xi - yi*yi) * P;
        }
    }


    if (diffuseEnv) {
        for (int i = 0; i < 9; ++i) {
            Llm[i] += shc_env[i] / (1 + smith_wo); // Problem here sometimes, seems to darken things way too much
        }
    }
}

/* E(wn) */
vec3 E(vec3 n) {
    float xn = n.x;
    float yn = n.y;
    float zn = n.z;

    return
            0.429043 * Llm[L22] * (xn*xn - yn*yn)
            + 0.743125 * Llm[L20] * zn*zn
            + 0.886227 * Llm[L00] - 0.247708 * Llm[L20]
            + 0.858086 * (Llm[L2_2]*xn*yn + Llm[L21]*xn*zn + Llm[L2_1]*yn*zn)
            + 1.023328 * (Llm[L11]*xn + Llm[L1_1]*yn + Llm[L10]*zn)
            ;
}

vec3 E_mat(vec3 n) {
    vec3 ret;

    vec4 v = vec4(n, 1);

    ret.x = dot(v, (hred) * v);
    ret.y = dot(v, (hgreen) * v);
    ret.z = dot(v, (hblue) * v);

    return ret;
}


vec3 roughDiffuse() {
    float sigma_x = sqrt(var_x);
    float sigma_y = sqrt(var_y);

    float r_xy = 1;

    if (sigma_x*sigma_y > 0)
        r_xy = c_xy / (sigma_x*sigma_y);

    float sq_one_minus_r_xy2 = sqrt(1 - r_xy*r_xy);

    float S = 0;
    vec3 I = vec3(0);


    for (int j = 0; j < nbSamples; ++j) {
        for (int k = 0; k < nbSamples; ++k) {
            float x = p[j]*sigma_x + E_xn;
            float y = (r_xy*p[j] + sq_one_minus_r_xy2*p[k]) * sigma_y + E_yn;


            // microfacet normal
            vec3 meso = getMesoWorld();

            // microfacet projected area
            float dotVN = dot(V, meso);
            float dotZN = dot(N, meso);
            float S_ = dotVN / dotZN;

            // irrandiance
            vec3 I_ = E(meso);

            // sum up
            S += w[j]*w[k] * S_ ;
            I += w[j]*w[k] * S_ * I_ ;
        }
    }

    return I / S;
}

/* LEADR SPECIFIC END */


float sinFromCos(float c) {
    return sqrt(1.0 - c*c);
}

float tanFromSinCos(float sine, float cosine) {
    return sine / cosine;
}

float Heavyside(float x) {
    return float(x > 0);
}

float D_Beckmann_1D_Walter07(float alpha_b, float NdotM) {
    float cos_theta_m = NdotM;
    float cos_theta_m4 = cos_theta_m*cos_theta_m*cos_theta_m*cos_theta_m;
    float tan_theta_m = tanFromSinCos(sinFromCos(cos_theta_m), cos_theta_m);
    float tan_theta_m2 = tan_theta_m*tan_theta_m;
    float alpha_b2 = alpha_b*alpha_b;

    float Heavyside_part = Heavyside(cos_theta_m);
    float exp_part = exp((-tan_theta_m2) / (alpha_b2));
    float div_part = M_PI * alpha_b2 * cos_theta_m4;

    float D_ = (Heavyside_part * exp_part) / div_part;

    return D_;
}

float D_Beckmann_1D_Schlick94(float rms_slope, float HdotN) {
    float t = HdotN;
    float t2 = t*t;
    float t4 = t2*t2;
    float m = rms_slope;
    float m2 = m*m;

    float exp_part = exp((t2 - 1.0) / (m2*t2));
    float div_part = m2*t4;

    float D_ = exp_part / div_part;

    return D_;
}

//rfa stands for Rational Fraction Approximation (see Schlick94)
float D_Beckmann_1D_Schlick94_optimized_rfa(float rms_slope, float HdotN) {
    float t = HdotN;
    float m = rms_slope;

    if (t < (1.0 - m)) {
        return 0.0;
    }

    float m2 = m*m;
    float m3 = m2*m;

    float x = t + m - 1.0;
    float x2 = x*x;

    float up_part = m3*x;
    float div_part = t*(m*x2 - x2 + m2);

    float D_ = up_part / div_part;

    return D_;
}


float G1_Beckmann_1D_Walter07(float alpha_b, float NdotV, float NdotN) {
    float cos_theta_n = NdotN;
    float cos_theta_v = NdotV;
    float tan_theta_v = tanFromSinCos(sinFromCos(cos_theta_v), cos_theta_v);

    float a = abs(1.0 / (alpha_b * tan_theta_v));
    a = min(a, 1.6);
    float a2 = a*a;

    float Heavyside_part = Heavyside(cos_theta_v / cos_theta_n);

    float simp_part = (3.535 * a + 2.181 * a2) / (1 + 2.276 * a + 2.577 * a2);

    float G_ = Heavyside_part * simp_part;

    return G_;
}


vec3 blinn_phong_calc_internal(vec3 lightDir, vec3 lightColor, vec3 normal) {
    float Id = clamp(dot(normal, lightDir), 0, 1);

    vec3 viewDir = normalize(eyePosition - inData.position);
    vec3 halfV = normalize(lightDir + viewDir);

    float Is = 0;
    if (Id > 0) {
        Is = pow(clamp(dot(normal, halfV), 0, 1), shininess);
    }

    // Replace the 3 colors by light ambiant, diffuse and specular intensity respectively
    float tmpAmbientFactor = 0.2;
    return Is * lightColor*Id;
}

vec3 blinn_phong_calc(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}

vec3 blinn_phong_calc(PointLight light, vec3 normal) {
    vec3 lightDir = normalize(light.position - eyePosition);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}


void main( void )
{
    V = normalize(eyePosition - inData.position); // Initialize global V (view vector) for all further uses

    computeLeadrStatistics();

//    vec3 normal = 2.0 * texture(normalMapSampler, inData.texcoord).xyz - vec3(1.0, 1.0, 1.0);
//    normal = normalize(normal);
//    normal = normalize(mat3(worldTangent, worldBitangent, worldNormal) * normal);

    vec3 worldNormal = normalize(inData.normal);
    vec3 worldTangent = normalize(inData.tangent);
    vec3 worldBitangent = normalize(cross(worldNormal, worldTangent));

    N = worldNormal;
    T = worldTangent;
    B = worldBitangent;

    vec3 mesoN = getMesoWorld();

    N = getMesoWorld();


    vec3 texAlbedo = texture(texSampler, inData.texcoord).xyz;

    normalColor = vec4(getMesoWorld(), 1);
    texcoordColor = vec4(inData.texcoord, 0, 1);

    vec3 pointL = normalize(pointLight.position - inData.position);

    vec3 pointH = normalize(pointL + V);

//    pointL = normalize(vec3(1, 0, 0));

    vec3 H = normalize(pointL + V);

    vec3 L = pointL;

    vec3 C = vec3(0.72549019607, 0.94901960784, 1.0); // Diamond color

    algo2(pointL, pointLight.color / 10.0);

//    fragColor.xyz = vec3(D_beckmann(toTanSpace(H)));

    float NdotH = dot(mesoN, H);
    float NdotV = dot(mesoN, V);
    float VdotH = dot(V, H);
    float LdotH = dot(L, H);
    float NdotL = dot(mesoN, L);

    float fr = 0;

    vec3 finalColor = vec3(0);

    if (currentBRDF == 0) {
        if (specularDirect) {
            fr += roughSpecularPointLight(pointL) * fresnel_schlick(fresnel0, LdotH);

            finalColor += max(0, dot(N, L)) * fr * vec3(1);
        }

        if (specularEnv) {
            finalColor += roughSpecularCubeMap();// * fresnel_schlick(fresnel0, NdotV);
        }

        if (diffuse) {
            finalColor += 1.0 * roughDiffuse();
        }
    }
    else if (currentBRDF == 1) {
        float alpha_b = 0.3;

        float F_ = fresnel_schlick(fresnel0, LdotH);
        float D_ = D_Beckmann_1D_Walter07(alpha_b, NdotH);
//        float D_ = D_Beckmann_1D_Schlick94(alpha_b, NdotH);
        float G_ = G1_Beckmann_1D_Walter07(alpha_b, NdotV, NdotH) * G1_Beckmann_1D_Walter07(alpha_b, NdotL, NdotH);

        fr = (F_ * D_ * G_) / (4 * abs(dot(mesoN, L)) * abs(dot(mesoN, V)));

        finalColor = vec3(fr) * clamp(NdotL, 0, 1);
    }
    else if (currentBRDF == 2) {
        finalColor = blinn_phong_calc(pointLight, mesoN);
    }
    else {

    }


//    fragColor.xyz += vec3(0.05); // FAKE AMBIANT

//    float albedo = 6.5;
//    fragColor.xyz = E(getMesoWorld()) * albedo;

    fragColor = vec4(finalColor, 1);

//    fragColor.xyz = vec3(disp);
}
