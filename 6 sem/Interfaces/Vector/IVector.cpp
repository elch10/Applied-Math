#include "IVector.hpp"
#include <cmath>
#include <memory>
#include <algorithm>
#include <iostream>

namespace
{
  class Vector: public IVector
  {
  public:
    static Vector* createVector(size_t dim, double* pData);
    Vector* clone() const;
    double getCoord(size_t index) const;
    int setCoord(size_t index, double value);
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

IVector* IVector::createVector(size_t dim, double* pData)
{
  return Vector::createVector(dim, pData);
}

IVector* IVector::add(IVector const* pOperand1, IVector const* pOperand2)
{
  if (!isEqualDim(pOperand1, pOperand2)) {
    return nullptr;
  }
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) + pOperand2->getCoord(i));
  }
  return cpy;
}

IVector* IVector::sub(IVector const* pOperand1, IVector const* pOperand2)
{
  if (!isEqualDim(pOperand1, pOperand2)) {
    return nullptr;
  }
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) - pOperand2->getCoord(i));
  }
  return cpy;
}

IVector* IVector::mul(IVector const* pOperand1, double scaleParam)
{
  IVector* cpy = pOperand1->clone();
  for (int i = 0; i < cpy->getDim(); ++i) {
    cpy->setCoord(i, pOperand1->getCoord(i) * scaleParam);
  }
  return cpy;
}

double IVector::mul(IVector const* pOperand1, IVector const* pOperand2)
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

Vector* Vector::createVector(size_t dim, double* pData)
{
  if (!dim || !pData || std::find(pData, pData + dim, NAN) != pData + dim) {
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

int Vector::setCoord(size_t index, double value)
{
  if (index >= dim_) {
    return -1;
  }
  components_[index] = value;
  return 0;
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
