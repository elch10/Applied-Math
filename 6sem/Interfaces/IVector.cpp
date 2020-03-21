#include "IVector.h"
#include <cmath>
#include <memory>
#include <algorithm>
#include <iostream>

namespace
{
  class Vector: public IVector
  {
  public:
    static Vector* createVector(size_t dim, double* pData, ILogger* pLogger);
    Vector* clone() const;
    
    double getCoord(size_t index) const;
    RESULT_CODE setCoord(size_t index, double value);
    size_t getDim() const;
    ~Vector() = default;
  protected:
    Vector(size_t dim, double *pData);
    std::unique_ptr<double[]> components_;
    size_t dim_;
  };

  bool isEqualDim(IVector const* pOperand1, IVector const* pOperand2)
  {
    return pOperand1->getDim() == pOperand2->getDim();
  }
}


IVector* IVector::createVector(size_t dim, double* pData, ILogger* pLogger)
{
  return Vector::createVector(dim, pData, pLogger);
}

IVector* IVector::add(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (!isEqualDim(pOperand1, pOperand2)) {
    pLogger->log("", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) + pOperand2->getCoord(i));
  }
  return cpy;
}

IVector* IVector::sub(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (!isEqualDim(pOperand1, pOperand2)) {
    pLogger->log("IVector::sub erorr:", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) - pOperand2->getCoord(i));
  }
  return cpy;
}

IVector* IVector::mul(IVector const* pOperand1, double scaleParam, ILogger* pLogger)
{
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) * scaleParam);
  }
  return cpy;
}

double IVector::mul(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (!isEqualDim(pOperand1, pOperand2)) {
    return NAN;
  }
  double sum = 0;
  for (int i = 0; i < pOperand1->getDim(); ++i) {
    sum += pOperand1->getCoord(i) * pOperand2->getCoord(i);
  }
  return sum;
}

RESULT_CODE IVector::equals(IVector const* pOperand1, 
                            IVector const* pOperand2, 
                            NORM norm, 
                            double tolerance,
                            bool* result, 
                            ILogger* pLogger)
{
  if (pOperand1->getDim() != pOperand2->getDim()) {
    pLogger->log("Differents dims in IVector::equals", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }
  double norm1 = IVector::norm(pOperand1, norm, pLogger);
  double norm2 = IVector::norm(pOperand2, norm, pLogger);
  if (std::isnan(norm1) || std::isnan(norm2)) {
    pLogger->log("Nan value in IVector::equals", RESULT_CODE::NAN_VALUE);
    return RESULT_CODE::NAN_VALUE;
  } else if (norm1 - norm2 > tolerance) {
    *result = false;
  } else {
    *result = true;
  }
  return RESULT_CODE::SUCCESS;
}

double IVector::norm(IVector const* pVector, NORM norm, ILogger* pLogger)
{
  double res = 0.;
  if (norm == NORM::NORM_1) {
    for (size_t i = 0; i < pVector->getDim(); ++i) {
      double value = pVector->getCoord(i);
      res += abs(value);
    }
  } else if (norm == NORM::NORM_2) {
    for (size_t i = 0; i < pVector->getDim(); ++i) {
      double value = pVector->getCoord(i);
      res += value * value;
    }
  } else if (norm == NORM::NORM_INF) {
    for (size_t i = 0; i < pVector->getDim(); ++i) {
      double value = pVector->getCoord(i);
      res = std::max(res, value);
    }
  } else {
    pLogger->log("Norm must be one of the declared in the NORM enum", RESULT_CODE::WRONG_ARGUMENT);
    res = NAN;
  }
  return res;
}

Vector* Vector::createVector(size_t dim, double* pData, ILogger* pLogger)
{
  if (!dim) {
    pLogger->log("Zero dimension", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }
  if (!pData) {
    pLogger->log("Nullptr in createVector", RESULT_CODE::BAD_REFERENCE);
    return nullptr;
  }
  bool (*isnan)(double) = std::isnan;
  if (std::find_if(pData, pData + dim, isnan) != pData + dim) {
    pLogger->log("", RESULT_CODE::NAN_VALUE);
    return nullptr;
  }
  double *cpy = new double[dim];
  std::copy_n(pData, dim, cpy);
  return new Vector(dim, cpy);
}

Vector* Vector::clone() const
{
  return createVector(dim_, components_.get());
}

double Vector::getCoord(size_t index) const
{
  if (index >= dim_) {
    return NAN;
  }
  return components_[index];
}

RESULT_CODE Vector::setCoord(size_t index, double value)
{
  if (index >= dim_) {
    return RESULT_CODE::OUT_OF_BOUNDS;
  }
  if (std::isnan(value)) {
    return RESULT_CODE::NAN_VALUE;
  }
  components_[index] = value;
  return RESULT_CODE::SUCCESS;
}

size_t Vector::getDim() const
{
  return dim_;
}

Vector::Vector(size_t dim, double *pData):
  dim_(dim), components_(pData)
{}

std::ostream &operator<< (std::ostream &out, const IVector *vec)
{
  if (!vec) {
    return out;
  }
  for (int i = 0; i < vec->getDim(); ++i) {
    out << vec->getCoord(i) << ' ';
  }
  return out;
}
