#include "ISet.h"
#include <vector>
#include <memory>

namespace
{
  class Set: public ISet
  {
  public:
    Set(ILogger *pLogger);
    ~Set() = default;

    RESULT_CODE insert(const IVector* pVector, IVector::NORM norm, double tolerance) override;
    RESULT_CODE get(IVector*& pVector, size_t index) const override;
    RESULT_CODE get(IVector*& pVector, IVector const* pSample, IVector::NORM norm, double tolerance) const override;
    size_t getDim() const override;
    size_t getSize() const override;
    void clear() override;
    RESULT_CODE erase(size_t index) override;
    RESULT_CODE erase(IVector const* pSample, IVector::NORM norm, double tolerance) override;
    Set* clone() const override;

  private:
    std::vector<std::unique_ptr<IVector>> vectors_;
    ILogger *logger_;
  };

  void validLogging(ILogger *pLogger, const char *msg, RESULT_CODE rc)
  {
    if (pLogger) {
      pLogger->log(msg, rc);
    }
  }

  bool isNullptr(const void * const arg, ILogger *pLogger)
  {
    if (!arg) {
      validLogging(pLogger, "Nullptr arguments", RESULT_CODE::BAD_REFERENCE);
      return true;
    }
    return false;
  }

  void copy(ISet const *from, ISet *to, IVector::NORM norm, double tolerance)
  {
    for (size_t i = 0; i < from->getSize(); ++i) {
      IVector *tmp;
      from->get(tmp, i);
      std::unique_ptr<IVector> lhs(tmp);
      to->insert(tmp, norm, tolerance);
    }
  }

}

ISet *ISet::createSet(ILogger* pLogger)
{
  return new Set(pLogger);
}

ISet* ISet::add(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  ISet *result = ISet::createSet(pLogger);
  copy(pOperand1, result, norm, tolerance);
  copy(pOperand2, result, norm, tolerance);
  return result;
}

ISet* ISet::intersect(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  ISet *result = ISet::createSet(pLogger);
  for (size_t i = 0; i < pOperand1->getSize(); ++i) {
    IVector *tmp;
    pOperand1->get(tmp, i);
    std::unique_ptr<IVector> lhs(tmp);
    
    RESULT_CODE rc = pOperand2->get(tmp, tmp, norm, tolerance);
    std::unique_ptr<IVector> rhs(tmp);

    if (rc == RESULT_CODE::SUCCESS) {
      result->insert(lhs.get(), norm, tolerance);
    }
  }
  return result;
}

ISet* ISet::sub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  ISet *result = ISet::createSet(pLogger);
  for (size_t i = 0; i < pOperand1->getSize(); ++i) {
    IVector *tmp;
    pOperand1->get(tmp, i);
    std::unique_ptr<IVector> lhs(tmp);
    
    RESULT_CODE rc = pOperand2->get(tmp, tmp, norm, tolerance);
    std::unique_ptr<IVector> rhs(tmp);

    if (rc == RESULT_CODE::NOT_FOUND) {
      result->insert(lhs.get(), norm, tolerance);
    }
  }
  return result;
}

ISet* ISet::symSub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger)
{
  if (isNullptr(pOperand1, pLogger) || isNullptr(pOperand2, pLogger)) {
    return nullptr;
  }

  std::unique_ptr<ISet> add(ISet::add(pOperand1, pOperand2, norm, tolerance, pLogger));
  std::unique_ptr<ISet> intersect(ISet::intersect(pOperand1, pOperand2, norm, tolerance, pLogger));
  return ISet::sub(add.get(), intersect.get(), norm, tolerance, pLogger);
}


Set::Set(ILogger *pLogger):
  logger_(pLogger)
{}


RESULT_CODE Set::insert(const IVector* pVector, IVector::NORM norm, double tolerance)
{
  if (isNullptr(pVector, logger_)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  IVector *vec;
  RESULT_CODE rc = get(vec, pVector, norm, tolerance);
  std::unique_ptr<IVector> tmp(vec);

  if (rc != RESULT_CODE::NOT_FOUND) {
    validLogging(logger_, "Element already is in Set", RESULT_CODE::MULTIPLE_DEFINITION);
    return RESULT_CODE::MULTIPLE_DEFINITION;
  }

  if (!vectors_.empty() && vectors_[0]->getDim() != pVector->getDim()) {
    validLogging(logger_, "Set must contain vectors with equal dims", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }

  vectors_.emplace_back(pVector->clone());
  return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::get(IVector*& pVector, size_t index) const
{
  if (index >= vectors_.size()) {
    pVector = nullptr;
    validLogging(logger_, "Index is out of bounds in Set::get", RESULT_CODE::OUT_OF_BOUNDS);
    return RESULT_CODE::OUT_OF_BOUNDS;
  }

  pVector = vectors_[index]->clone();
  return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::get(IVector*& pVector, IVector const* pSample, IVector::NORM norm, double tolerance) const
{
  if (isNullptr(pSample, logger_)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  for (const auto &vec: vectors_) {
    std::unique_ptr<IVector> sub(IVector::sub(pSample, vec.get(), logger_));
    if (sub->norm(norm) < tolerance) {
      pVector = vec->clone();
      return RESULT_CODE::SUCCESS;
    }
  }

  pVector = nullptr;
  return RESULT_CODE::NOT_FOUND;
}

size_t Set::getDim() const
{
  if (!vectors_.empty()) {
    return vectors_[0]->getDim();
  }
  return 0;
}

size_t Set::getSize() const
{
  return vectors_.size();
}

void Set::clear()
{
  vectors_.clear();
}

RESULT_CODE Set::erase(size_t index)
{
  if (index >= vectors_.size()) {
    validLogging(logger_, "Index is out of bounds in Set::get", RESULT_CODE::OUT_OF_BOUNDS);
    return RESULT_CODE::OUT_OF_BOUNDS;
  }

  vectors_.erase(vectors_.begin() + index);
  return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::erase(IVector const* pSample, IVector::NORM norm, double tolerance)
{
  if (isNullptr(pSample, logger_)) {
    return RESULT_CODE::WRONG_ARGUMENT;
  }

  for (int i = 0; i < vectors_.size(); ++i) {
    std::unique_ptr<IVector> sub(IVector::sub(pSample, vectors_[i].get(), logger_));
    if (sub->norm(norm) < tolerance) {
      vectors_.erase(vectors_.begin() + i);
      return RESULT_CODE::SUCCESS;
    }
  }

  return RESULT_CODE::NOT_FOUND;
}

Set* Set::clone() const
{
  Set *n = new Set(logger_);
  n->vectors_.reserve(vectors_.size());
  for (const auto &e: vectors_) {
    n->vectors_.emplace_back(e->clone());
  }
  return n;
}

ISet::~ISet() {}
