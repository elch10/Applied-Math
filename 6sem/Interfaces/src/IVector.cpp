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
    Vector* clone() const override;
    
    double norm(NORM norm) const override;
    double getCoord(size_t index) const override;
    RESULT_CODE setCoord(size_t index, double value) override;
    size_t getDim() const override;
    ~Vector() = default;

  protected:
    Vector(size_t dim, double *pData);
    std::unique_ptr<double[]> components_;
    size_t dim_;
  private:
    static Vector* createHelper(size_t dim, double *pData);
  };

  bool isEqualDim(IVector const* pOperand1, IVector const* pOperand2)
  {
    return pOperand1->getDim() == pOperand2->getDim();
  }

  RESULT_CODE validLogging(ILogger *pLogger, const char *msg, RESULT_CODE rc)
  {
    if (pLogger) {
      pLogger->log(msg, rc);
    }
    return rc;
  }

  bool isNullptr(const IVector * const vec, ILogger *pLogger)
  {
    if (!vec) {
      validLogging(pLogger, "Nullptr arguments", RESULT_CODE::BAD_REFERENCE);
      return true;
    }
    return false;
  }

  void resetIfNan(std::unique_ptr<IVector> &vec, ILogger *pLogger)
  {
    bool nan = false;
    for (size_t i = 0; i < vec->getDim(); ++i) {
      if (std::isnan(vec->getCoord(i))) {
        vec.reset();
        validLogging(pLogger, "Nan encountered when calculating", RESULT_CODE::NAN_VALUE);
        break;
      }
    }
  }

}


IVector* IVector::createVector(size_t dim, double* pData, ILogger* pLogger)
{
  return Vector::createVector(dim, pData, pLogger);
}

IVector* IVector::add(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  if (!isEqualDim(pOperand1, pOperand2)) {
    validLogging(pLogger, "Different dims in IVector::add", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  std::unique_ptr<IVector> cpy(pOperand1->clone());
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) + pOperand2->getCoord(i));
  }

  resetIfNan(cpy, pLogger);
  return cpy.release();
}

IVector* IVector::sub(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  if (!isEqualDim(pOperand1, pOperand2)) {
    validLogging(pLogger, "IVector::sub erorr:", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  std::unique_ptr<IVector> cpy(pOperand1->clone());

  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) - pOperand2->getCoord(i));
  }

  resetIfNan(cpy, pLogger);
  return cpy.release();
}

IVector* IVector::mul(IVector const* pOperand1, double scaleParam, ILogger* pLogger)
{
  if (std::isnan(scaleParam)) {
    validLogging(pLogger, "Nan value in scalar", RESULT_CODE::NAN_VALUE);
    return nullptr;
  }

  if (isNullptr(pOperand1, pLogger)) {
    return nullptr;
  }

  std::unique_ptr<IVector> cpy(pOperand1->clone());

  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) * scaleParam);
  }

  resetIfNan(cpy, pLogger);
  return cpy.release();
}

double IVector::mul(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return NAN;
  }

  if (!isEqualDim(pOperand1, pOperand2)) {
    validLogging(pLogger, "Differents dims in IVector::mul", RESULT_CODE::WRONG_DIM);
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
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  if (std::isnan(tolerance)) {
    return validLogging(pLogger, "Nan value of tolerance", RESULT_CODE::NAN_VALUE);
  }

  if (!isEqualDim(pOperand1, pOperand2)) {
    validLogging(pLogger, "Differents dims in IVector::equals", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }

  double norm1 = pOperand1->norm(norm);
  double norm2 = pOperand2->norm(norm);
  if (std::isnan(norm1) || std::isnan(norm2)) {
    return validLogging(pLogger, "Nan value in IVector::equals", RESULT_CODE::NAN_VALUE);
  } else if (std::abs(norm1 - norm2) > tolerance) {
    *result = false;
  } else {
    *result = true;
  }
  return RESULT_CODE::SUCCESS;
}

Vector *Vector::createHelper(size_t dim, double *pData)
{
  double *cpy = new double[dim];
  std::copy_n(pData, dim, cpy);
  return new Vector(dim, cpy);
}

Vector* Vector::createVector(size_t dim, double* pData, ILogger* pLogger)
{
  if (!dim) {
    validLogging(pLogger, "Zero dimension", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  if (!pData) {
    validLogging(pLogger, "Nullptr in createVector", RESULT_CODE::BAD_REFERENCE);
    return nullptr;
  }

  bool (*isnan)(double) = std::isnan;
  if (std::find_if(pData, pData + dim, isnan) != pData + dim) {
    validLogging(pLogger, "Nan in data", RESULT_CODE::NAN_VALUE);
    return nullptr;
  }

  return createHelper(dim, pData);
}

Vector* Vector::clone() const
{
  return createHelper(dim_, components_.get());
}

double Vector::getCoord(size_t index) const
{
  if (index >= dim_) {
    return NAN;
  }
  return components_[index];
}

double Vector::norm(IVector::NORM norm) const
{
  double res = 0.;
  if (norm == NORM::NORM_1) {
    for (size_t i = 0; i < dim_; ++i) {
      double value = components_[i];
      res += abs(value);
    }
  } else if (norm == NORM::NORM_2) {
    for (size_t i = 0; i < dim_; ++i) {
      double value = components_[i];
      res += value * value;
    }
    res = std::sqrt(res);
  } else if (norm == NORM::NORM_INF) {
    for (size_t i = 0; i < dim_; ++i) {
      double value = abs(components_[i]);
      res = std::max(res, value);
    }
  } else {
    res = NAN;
  }
  return res;
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

IVector::~IVector() {}
