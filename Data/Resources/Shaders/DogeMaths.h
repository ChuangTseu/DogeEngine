#pragma once

#define M_PI 3.14159265358979323846

float sinFromCos(float c) {
    return sqrt(1.0 - c*c);
}

float tanFromSinCos(float sine, float cosine) {
    return sine / cosine;
}

float Heavyside(float x) {
    return float(x > 0);
}

