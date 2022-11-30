#pragma once
#include "pch.h"
#include "Random.h"

class PDF {
public:
	virtual double Value(const glm::vec3& direction) const = 0;
	virtual glm::vec3 Generate() const = 0;
};

class CosinePDF : public PDF {
public:
    CosinePDF(const glm::vec3& w);

    virtual double Value(const glm::vec3& direction) const override;
    virtual glm::vec3 Generate() const override;

public:
    ONB uvw;
};