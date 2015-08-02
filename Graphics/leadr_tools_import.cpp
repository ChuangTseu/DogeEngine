#include "leadr_tools_import.h"




bool importCoeffs(const char *filename, SphericalHarmonicsCoeffs &shc) {
    FILE* fp = NULL;

    fp = fopen(filename, "rb");

    if (!fp) {
        std::cerr << "Error opening file " << filename << '\n';

        return false;
    }

    fread((void*) shc.coeffs, sizeof(float), 9*3, fp);

    fclose(fp);

    return true;
}


void computeMatrixRepresentation(SphericalHarmonicsCoeffs &shc) {
    toSingleChannelMatrix(shc.coeffs, shc.hred, 0);
    toSingleChannelMatrix(shc.coeffs, shc.hgreen, 1);
    toSingleChannelMatrix(shc.coeffs, shc.hblue, 2);
}


void toSingleChannelMatrix(SimpleShc coeffs, mat4 &m, int channel) {
    float c1,c2,c3,c4,c5 ;
    c1 = 0.429043f ; c2 = 0.511664f ;
    c3 = 0.743125f ; c4 = 0.886227f ; c5 = 0.247708f ;

    m[0][0] = c1*coeffs[8][channel] ; /* c1 L_{22}  */
    m[0][1] = c1*coeffs[4][channel] ; /* c1 L_{2-2} */
    m[0][2] = c1*coeffs[7][channel] ; /* c1 L_{21}  */
    m[0][3] = c2*coeffs[3][channel] ; /* c2 L_{11}  */

    m[1][0] = c1*coeffs[4][channel] ; /* c1 L_{2-2} */
    m[1][1] = -c1*coeffs[8][channel]; /*-c1 L_{22}  */
    m[1][2] = c1*coeffs[5][channel] ; /* c1 L_{2-1} */
    m[1][3] = c2*coeffs[1][channel] ; /* c2 L_{1-1} */

    m[2][0] = c1*coeffs[7][channel] ; /* c1 L_{21}  */
    m[2][1] = c1*coeffs[5][channel] ; /* c1 L_{2-1} */
    m[2][2] = c3*coeffs[6][channel] ; /* c3 L_{20}  */
    m[2][3] = c2*coeffs[2][channel] ; /* c2 L_{10}  */

    m[3][0] = c2*coeffs[3][channel] ; /* c2 L_{11}  */
    m[3][1] = c2*coeffs[1][channel] ; /* c2 L_{1-1} */
    m[3][2] = c2*coeffs[2][channel] ; /* c2 L_{10}  */
    m[3][3] = c4*coeffs[0][channel] - c5*coeffs[6][channel] ;
}


void printCoeffs(const SimpleShc coeffs) {
    printf("\n         Lighting Coefficients\n\n") ;
    printf("(l,m)       RED        GREEN     BLUE\n") ;

    printf("L_{0,0}   %9.6f %9.6f %9.6f\n",
           coeffs[0][0],coeffs[0][1],coeffs[0][2]) ;
    printf("L_{1,-1}  %9.6f %9.6f %9.6f\n",
           coeffs[1][0],coeffs[1][1],coeffs[1][2]) ;
    printf("L_{1,0}   %9.6f %9.6f %9.6f\n",
           coeffs[2][0],coeffs[2][1],coeffs[2][2]) ;
    printf("L_{1,1}   %9.6f %9.6f %9.6f\n",
           coeffs[3][0],coeffs[3][1],coeffs[3][2]) ;
    printf("L_{2,-2}  %9.6f %9.6f %9.6f\n",
           coeffs[4][0],coeffs[4][1],coeffs[4][2]) ;
    printf("L_{2,-1}  %9.6f %9.6f %9.6f\n",
           coeffs[5][0],coeffs[5][1],coeffs[5][2]) ;
    printf("L_{2,0}   %9.6f %9.6f %9.6f\n",
           coeffs[6][0],coeffs[6][1],coeffs[6][2]) ;
    printf("L_{2,1}   %9.6f %9.6f %9.6f\n",
           coeffs[7][0],coeffs[7][1],coeffs[7][2]) ;
    printf("L_{2,2}   %9.6f %9.6f %9.6f\n",
           coeffs[8][0],coeffs[8][1],coeffs[8][2]) ;
}

void printMatricesToGlslDeclaration(const SphericalHarmonicsCoeffs& shc) {
    printf("\nconst mat4 hred = mat4(\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        printf("    ");
        for (int j = 0 ; j < 4 ; j++) {
            if (i == 3 && j == 3) {
                printf("%9.6f ", shc.hred[i][j]) ;
            }
            else {
                printf("%9.6f, ", shc.hred[i][j]) ;
            }
        }
        printf("\n") ;
    }
    printf(") ;\n");

    printf("\nconst mat4 hgreen = mat4(\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        printf("    ");
        for (int j = 0 ; j < 4 ; j++) {
            if (i == 3 && j == 3) {
                printf("%9.6f ", shc.hgreen[i][j]) ;
            }
            else {
                printf("%9.6f, ", shc.hgreen[i][j]) ;
            }
        }
        printf("\n") ;
    }
    printf(") ;\n");

    printf("\nconst mat4 hblue = mat4(\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        printf("    ");
        for (int j = 0 ; j < 4 ; j++) {
            if (i == 3 && j == 3) {
                printf("%9.6f ", shc.hblue[i][j]) ;
            }
            else {
                printf("%9.6f, ", shc.hblue[i][j]) ;
            }
        }
        printf("\n") ;
    }
    printf(") ;\n");
}


struct leadr1_pixel {
    float dx;
    float dy;
    float dxy;
    float dxx;
};

#pragma pack(push, 1) // exact fit - no padding
struct leadr2_pixel {
    float dyy;
    unsigned char disp;
};
#pragma pack(pop) //back to whatever the previous packing mode was

struct RGBA32F_pixel {
    float r;
    float g;
    float b;
    float a;
};

bool importLeadrTexture(const char *filename, Texture &leadr1, Texture &leadr2) {
    FILE* fp = NULL;

    fp = fopen(filename, "rb");

    if (!fp) {
        std::cerr << "Error opening file " << filename << '\n';

        return false;
    }

    int width, height;

    fread((void*) &width, sizeof(int), 1, fp);
    fread((void*) &height, sizeof(int), 1, fp);

    size_t size = width * height;

    std::cout << "Width : " << width << ", Height : " << height << '\n';

    std::cout << '\n';

    std::cout << "sizeof(leadr2_pixel) : " << sizeof(leadr2_pixel) << '\n';

    RGBA32F_pixel* textureData = new RGBA32F_pixel[size];


    leadr1_pixel* leadr1Data = new leadr1_pixel[size];

    fread((void*) leadr1Data, sizeof(leadr1_pixel), size, fp);


    // Copy from leadr1Data to textureData is a "no-op" here
    leadr1.loadFromBlob(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, leadr1Data);


    leadr2_pixel* leadr2Data = new leadr2_pixel[size];

    fread((void*) leadr2Data, sizeof(leadr2_pixel), size, fp);

    // Convert leadr2Data to textureData
    for (size_t i = 0; i < size; ++i) {
        textureData[i].r = leadr2Data[i].dyy;
        textureData[i].g = (float) (leadr2Data[i].disp / 255.f);

        textureData[i].b = 0.f;
        textureData[i].a = 0.f;
    }

    leadr2.loadFromBlob(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, textureData);

    fclose(fp);



    delete[] textureData;
	delete[] leadr1Data;
	delete[] leadr2Data;

    return true;

    return true;
}
