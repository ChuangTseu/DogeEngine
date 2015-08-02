#ifndef LEADR_TOOLS_IMPORT_H
#define LEADR_TOOLS_IMPORT_H

#include "MathsTools/vec3.h"
#include "MathsTools/mat4.h"

#include <cstdio>
#include <iostream>

#include "Graphics/texture.h"

typedef Color3f SimpleShc[9];

struct SphericalHarmonicsCoeffs {
    Color3f coeffs[9];

    mat4 hred;
    mat4 hgreen;
    mat4 hblue;
};

bool importCoeffs(const char *filename, SphericalHarmonicsCoeffs &shc);

void computeMatrixRepresentation(SphericalHarmonicsCoeffs &shc);

void toSingleChannelMatrix(SimpleShc coeffs, mat4 &m, int channel);

void printCoeffs(const SimpleShc coeffs);

void printMatricesToGlslDeclaration(const SphericalHarmonicsCoeffs& shc);


bool importLeadrTexture(const char *filename, Texture &leadr1, Texture &leadr2);




#endif // LEADR_TOOLS_IMPORT_H
