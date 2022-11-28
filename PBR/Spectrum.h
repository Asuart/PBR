#pragma once
#include "pch.h"

const int32_t sampledLambdaStart = 400;
const int32_t sampledLambdaEnd = 700;
const int32_t nSpectralSamples = 60;

static const int nCIESamples = 471;
extern const float CIE_X[nCIESamples];
extern const float CIE_Y[nCIESamples];
extern const float CIE_Z[nCIESamples];
extern const float CIE_lambda[nCIESamples];

static const int nRGB2SpectSamples = 32;
extern const float RGB2SpectLambda[nRGB2SpectSamples];
extern const float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const float RGBRefl2SpectBlue[nRGB2SpectSamples];

extern const float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const float RGBIllum2SpectBlue[nRGB2SpectSamples];


inline float Lerp(float t, float v1, float v2) {
    return (1 - t) * v1 + t * v2;
}

inline void XYZToRGB(const float xyz[3], float rgb[3]) {
    rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
    rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
    rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
    xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
    xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
    xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

class Spectrum;

template <int32_t nSpectrumSamples> 
class CoefficientSpectrum {
public:
    static const int32_t nSamples = nSpectrumSamples;

    CoefficientSpectrum(float v = 0.f) {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            c[i] = v;
    }

    CoefficientSpectrum& operator+=(const CoefficientSpectrum& s2) {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            c[i] += s2.c[i];
        return *this;
    }

    CoefficientSpectrum operator+(const CoefficientSpectrum& s2) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] += s2.c[i];
        return ret;
    }

    CoefficientSpectrum operator-(const CoefficientSpectrum& s2) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] -= s2.c[i];
        return ret;
    }

    CoefficientSpectrum operator/(const CoefficientSpectrum& s2) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] /= s2.c[i];
        return ret;
    }

    CoefficientSpectrum operator*(const CoefficientSpectrum& sp) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] *= sp.c[i];
        return ret;
    }

    CoefficientSpectrum& operator*=(const CoefficientSpectrum& sp) {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            c[i] *= sp.c[i];
        return *this;
    }

    CoefficientSpectrum operator*(float a) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] *= a;
        return ret;
    }

    CoefficientSpectrum& operator*=(float a) {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            c[i] *= a;
        return *this;
    }

    friend inline CoefficientSpectrum operator*(float a, const CoefficientSpectrum& s) {
        return s * a;
    }

    CoefficientSpectrum operator/(float a) const {
        CoefficientSpectrum ret = *this;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] /= a;
        Assert(!ret.HasNaNs());
        return ret;
    }

    CoefficientSpectrum& operator/=(float a) {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            c[i] /= a;
        return *this;
    }

    bool operator==(const CoefficientSpectrum& sp) const {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            if (c[i] != sp.c[i]) return false;
        return true;
    }

    bool operator!=(const CoefficientSpectrum& sp) const {
        return !(*this == sp);
    }

    bool IsBlack() const {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            if (c[i] != 0.) return false;
        return true;
    }

    friend CoefficientSpectrum Sqrt(const CoefficientSpectrum& s) {
        CoefficientSpectrum ret;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = std::sqrt(s.c[i]);
        return ret;
    }

    template <int32_t n> friend inline CoefficientSpectrum<n> Pow(const CoefficientSpectrum<n>& s, float e);

    CoefficientSpectrum operator-() const {
        CoefficientSpectrum ret;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = -c[i];
        return ret;
    }

    friend CoefficientSpectrum Exp(const CoefficientSpectrum& s) {
        CoefficientSpectrum ret;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = std::exp(s.c[i]);
        return ret;
    }

    friend std::ostream& operator<<(std::ostream& os, const CoefficientSpectrum& s) {
        os << "[";
        for (int32_t i = 0; i < nSpectrumSamples; ++i) {
            os << s.c[i];
            if (i + 1 < nSpectrumSamples)
                os << ", ";
        }
        os << "]";
        return os;
    }

    CoefficientSpectrum Clamp(float low = 0, float high = INFINITY) const {
        CoefficientSpectrum ret;
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = ::Clamp(c[i], low, high);
        return ret;
    }

    bool HasNaNs() const {
        for (int32_t i = 0; i < nSpectrumSamples; ++i)
            if (std::isnan(c[i])) return true;
        return false;
    }

    float& operator[](int32_t i) {
        return c[i];
    }

    float operator[](int32_t i) const {
        return c[i];
    }

    inline Spectrum Lerp(float t, const Spectrum& s1, const Spectrum& s2) {
        return (1 - t) * s1 + t * s2;
    }

protected:
    float c[nSpectrumSamples];

};




float AverageSpectrumSamples(const float* lambda, const float* vals, int n, float lambdaStart, float lambdaEnd) {
    if (lambdaEnd <= lambda[0])     return vals[0];
    if (lambdaStart >= lambda[n - 1]) return vals[n - 1];
    if (n == 1) return vals[0];

    float sum = 0;
    if (lambdaStart < lambda[0])
        sum += vals[0] * (lambda[0] - lambdaStart);
    if (lambdaEnd > lambda[n - 1])
        sum += vals[n - 1] * (lambdaEnd - lambda[n - 1]);

    int i = 0;
    while (lambdaStart > lambda[i + 1]) ++i;


    auto interp = [lambda, vals](float w, int i) {
        return Lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]), vals[i], vals[i + 1]);
    };
    for (; i + 1 < n && lambdaEnd >= lambda[i]; ++i) {
        float segLambdaStart = std::max(lambdaStart, lambda[i]);
        float segLambdaEnd = std::min(lambdaEnd, lambda[i + 1]);
        sum += 0.5 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i)) *
            (segLambdaEnd - segLambdaStart);
    }

    return sum / (lambdaEnd - lambdaStart);
}


class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples> {
public:
    SampledSpectrum(float v = 0.f) 
        : CoefficientSpectrum(v) {}
    SampledSpectrum(const CoefficientSpectrum<nSpectralSamples>& v)
        : CoefficientSpectrum<nSpectralSamples>(v) {}

    static SampledSpectrum FromSampled(const float* lambda, const float* v, int32_t n) {
        /*
        if (!SpectrumSamplesSorted(lambda, v, n)) {
            std::vector<float> slambda(&lambda[0], &lambda[n]);
            std::vector<float> sv(&v[0], &v[n]);
            SortSpectrumSamples(&slambda[0], &sv[0], n);
            return FromSampled(&slambda[0], &sv[0], n);
        }
        */

        SampledSpectrum r;
        for (int32_t i = 0; i < nSpectralSamples; ++i) {
            float lambda0 = ::Lerp(float(i) / float(nSpectralSamples), (float)sampledLambdaStart, (float)sampledLambdaEnd);
            float lambda1 = ::Lerp(float(i + 1) / float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
            r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);

        }
        return r;
    }

    static void Init() {
            for (int32_t i = 0; i < nSpectralSamples; ++i) {
                float wl0 = ::Lerp(float(i) / float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
                float wl1 = ::Lerp(float(i + 1) / float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
                X.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, nCIESamples, wl0, wl1);
                Y.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, nCIESamples, wl0, wl1);
                Z.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, nCIESamples, wl0, wl1);
            }

            for (int32_t i = 0; i < nSpectralSamples; ++i) {
                float wl0 = ::Lerp(float(i) / float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
                float wl1 = ::Lerp(float(i + 1) / float(nSpectralSamples), sampledLambdaStart, sampledLambdaEnd);
                rgbRefl2SpectWhite.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectWhite, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectCyan.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectCyan, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectMagenta.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectMagenta, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectYellow.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectYellow, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectRed.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectRed, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectGreen.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectGreen, nRGB2SpectSamples, wl0, wl1);
                rgbRefl2SpectBlue.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectBlue, nRGB2SpectSamples, wl0, wl1);

                rgbIllum2SpectWhite.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectWhite, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectCyan.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectCyan, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectMagenta.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectMagenta, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectYellow.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectYellow, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectRed.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectRed, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectGreen.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectGreen, nRGB2SpectSamples, wl0, wl1);
                rgbIllum2SpectBlue.c[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectBlue, nRGB2SpectSamples, wl0, wl1);
            }

    }

    void ToXYZ(float xyz[3]) const {
        xyz[0] = xyz[1] = xyz[2] = 0.f;
        for (int i = 0; i < nSpectralSamples; ++i) {
            xyz[0] += X.c[i] * c[i];
            xyz[1] += Y.c[i] * c[i];
            xyz[2] += Z.c[i] * c[i];
        }
        float scale = float(sampledLambdaEnd - sampledLambdaStart) / float(CIE_Y_integral * nSpectralSamples);
        xyz[0] *= scale;
        xyz[1] *= scale;
        xyz[2] *= scale;
    }

    float y() const {
        float yy = 0.f;
        for (int i = 0; i < nSpectralSamples; ++i)
            yy += Y.c[i] * c[i];
        return yy * float(sampledLambdaEnd - sampledLambdaStart) / float(nSpectralSamples);
    }

    void ToRGB(float rgb[3]) const {
        float xyz[3];
        ToXYZ(xyz);
        XYZToRGB(xyz, rgb);
    }

    RGBSpectrum ToRGBSpectrum() const;

    static SampledSpectrum FromRGB(const float rgb[3], SpectrumType type = SpectrumType::Illuminant);
    static SampledSpectrum FromXYZ(const float xyz[3], SpectrumType type = SpectrumType::Reflectance) {
        float rgb[3];
        XYZToRGB(xyz, rgb);
        return FromRGB(rgb, type);
    }
    SampledSpectrum(const RGBSpectrum& r, SpectrumType type = SpectrumType::Reflectance);

private:
    static SampledSpectrum X, Y, Z;
    static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
    static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
    static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
    static SampledSpectrum rgbRefl2SpectBlue;
    static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
    static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
    static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
    static SampledSpectrum rgbIllum2SpectBlue;
};
