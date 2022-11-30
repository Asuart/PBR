#include "pch.h"
#include "PDF.h"


CosinePDF::CosinePDF(const glm::vec3& w) {
    uvw.build_from_w(w);
}

double CosinePDF::Value(const glm::vec3& direction) const {
    float cosine = glm::dot(RandomInHemisphere(direction), uvw.w());
    return (cosine <= 0) ? 0 : cosine / PI;
}

glm::vec3 CosinePDF::Generate() const {
    return uvw.local(RandomCosineDirection());
}