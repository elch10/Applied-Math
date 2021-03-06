#include "ICompact.h"
#include "IVector.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace
{
  class Compact: public ICompact
  {
  public:
    Compact(IVector * begin, IVector * end, ILogger *logger);

    IVector* getBegin() const override;
    IVector* getEnd() const override;

    iterator* end(IVector const* const step = 0) override;
    iterator* begin(IVector const* const step = 0) override;

    RESULT_CODE isContains(IVector const* const vec, bool& result) const override;
    RESULT_CODE isSubSet(ICompact const* const other, bool& result) const override;
    RESULT_CODE isIntersects(ICompact const* const other, bool& result) const override;

    size_t getDim() const override;
    ICompact* clone() const override;

    ~Compact() = default;

    class iterator_impl: public iterator
    {
    public:
      iterator_impl(IVector const * const startPoint, IVector const * const endPoint, IVector * step);

      RESULT_CODE doStep() override;

      IVector* getPoint() const override;

      RESULT_CODE setDirection(IVector const* const dir) override;
    private:
      std::unique_ptr<IVector> currentPoint_;
      std::unique_ptr<const IVector> step_, dir_;
      const IVector * const startPoint_, * const endPoint_;

      RESULT_CODE doStepHelper(size_t dimByPriority);
    };

  private:
    std::unique_ptr<IVector> begin_, end_;
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

  bool areSame(double a, double b)
  {
    return std::fabs(a - b) < 1e-5;
  }

}


ICompact* ICompact::createCompact(IVector const * const begin, IVector const * const end, ILogger *logger)
{
  if (isNullptr(begin, logger) || isNullptr(end, logger)) {
    return nullptr;
  }

  bool all_valid = true;
  if (begin->getDim() != end->getDim()) {
    validLogging(logger, "Dimensions must be equal for begin and end", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  for (int i = 0; i < begin->getDim(); ++i) {
    all_valid &= begin->getCoord(i) <= end->getCoord(i);
  }

  if (!all_valid) {
    validLogging(logger, "Not all begin coords are less or equal to end coords", RESULT_CODE::WRONG_ARGUMENT);
    return nullptr;
  }

  return new Compact(begin->clone(), end->clone(), logger);
}


ICompact* ICompact::intersection(ICompact const * const left, ICompact const * const right, ILogger *logger)
{
  if (isNullptr(left, logger) || isNullptr(right, logger)) {
    return nullptr;
  }

  std::unique_ptr<IVector> begin(left->getBegin()), rightBegin(right->getBegin());
  if (begin->getDim() != rightBegin->getDim()) {
    validLogging(logger, "Dimensions must be equal for two different compacts", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  std::unique_ptr<IVector> end(left->getEnd()), rightEnd(right->getEnd());
  for (int i = 0; i < begin->getDim(); ++i) {
    double r = std::min(end->getCoord(i), rightEnd->getCoord(i)),
           l = std::max(begin->getCoord(i), rightBegin->getCoord(i));

    if (l > r) {
      validLogging(logger, "Intersection is empty", RESULT_CODE::WRONG_ARGUMENT);
      return nullptr;
    }
    begin->setCoord(i, l);
    end->setCoord(i, r);
  }

  return new Compact(begin.release(), end.release(), logger);
}


ICompact* ICompact::add(ICompact const* const left, ICompact const* const right, ILogger *logger)
{
  if (isNullptr(left, logger) || isNullptr(right, logger)) {
    return nullptr;
  }

  if (left->getDim() != right->getDim()) {
    validLogging(logger, "Different dims of left and right in ICompact::add", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }
  
  std::unique_ptr<IVector> leftBegin(left->getBegin()), leftEnd(left->getEnd()),
    rightBegin(right->getBegin()), rightEnd(right->getEnd());

  size_t nEqualEdges = 0;
  for (size_t i = 0; i < left->getDim(); ++i) {
    nEqualEdges += areSame(leftBegin->getCoord(i), rightBegin->getCoord(i)) &&
                   areSame(leftEnd->getCoord(i), rightEnd->getCoord(i));
  }

  bool isIntersects = false;
  right->isIntersects(left, isIntersects);

  bool isSubSet = false, tmp = false;
  right->isSubSet(left, tmp);
  isSubSet |= tmp;
  left->isSubSet(right, tmp);

  if (!isIntersects || (!isSubSet && nEqualEdges < left->getDim() - 1)) {
    validLogging(logger, "Union on this compacts is not convex", RESULT_CODE::WRONG_ARGUMENT);
    return nullptr;
  }

  return makeConvex(left, right, logger);
}


ICompact* ICompact::makeConvex(ICompact const* const left, ICompact const* const right, ILogger *logger)
{
  if (isNullptr(left, logger) || isNullptr(right, logger)) {
    return nullptr;
  }

  if (left->getDim() != right->getDim()) {
    validLogging(logger, "Different dims in makeConvex", RESULT_CODE::WRONG_DIM);
    return nullptr;
  }

  std::unique_ptr<IVector> begin(left->getBegin()),
                           rightBegin(right->getBegin()),
                           end(left->getEnd()),
                           rightEnd(right->getEnd());

  for (int i = 0; i < begin->getDim(); ++i) {
    double l = std::min(begin->getCoord(i), rightBegin->getCoord(i)),
           r = std::max(end->getCoord(i), rightEnd->getCoord(i));

    begin->setCoord(i, l);
    end->setCoord(i, r);
  }

  return new Compact(begin.release(), end.release(), logger);
}


Compact::Compact(IVector * begin, IVector * end, ILogger *logger):
  begin_(begin), end_(end), logger_(logger)
{}


IVector* Compact::getBegin() const
{
  return begin_->clone();
}


IVector* Compact::getEnd() const
{
  return end_->clone();
}


Compact::iterator* Compact::end(IVector const* const step)
{
  if (isNullptr(step, logger_)) {
    return nullptr;
  }

  bool all_valid = true;
  for (int i = 0; i < step->getDim(); ++i) {
    all_valid &= step->getCoord(i) < 0;
  }

  if (!all_valid) {
    validLogging(logger_, "For end iterator all components of the step must be negative", RESULT_CODE::WRONG_ARGUMENT);
    return nullptr;
  }

  return new iterator_impl(end_.get(), begin_.get(), step->clone());
}


Compact::iterator* Compact::begin(IVector const* const step)
{
  if (isNullptr(step, logger_)) {
    return nullptr;
  }

  bool all_valid = true;
  for (int i = 0; i < step->getDim(); ++i) {
    all_valid &= step->getCoord(i) > 0;
  }

  if (!all_valid) {
    validLogging(logger_, "For begin iterator all components of the step must be positive", RESULT_CODE::WRONG_ARGUMENT);
    return nullptr;
  }

  return new iterator_impl(begin_.get(), end_.get(), step->clone());
}

RESULT_CODE Compact::isContains(IVector const* const vec, bool& result) const
{
  if (isNullptr(vec, logger_)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  if (vec->getDim() != getDim()) {
    validLogging(logger_, "Unequal dimensions in isContains", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }

  for (size_t i = 0; i < vec->getDim(); ++i) {
    if (vec->getCoord(i) < begin_->getCoord(i) ||
        vec->getCoord(i) > end_->getCoord(i)) {
      result = false;
      return RESULT_CODE::SUCCESS;
    }
  }

  result = true;
  return RESULT_CODE::SUCCESS;
}


RESULT_CODE Compact::isSubSet(ICompact const* const other, bool& result) const
{
  if (isNullptr(other, logger_)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  if (other->getDim() != getDim()) {
    validLogging(logger_, "Unequal dimensions in isSubSet", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }

  std::unique_ptr<IVector> otherBegin(other->getBegin()), otherEnd(other->getEnd());

  for (int i = 0; i < begin_->getDim(); ++i) {
    if (begin_->getCoord(i) > otherBegin->getCoord(i) ||
        end_->getCoord(i) < otherEnd->getCoord(i)) {
      result = false;
      return RESULT_CODE::SUCCESS;
    }
  }
  result = true;
  return RESULT_CODE::SUCCESS;
}


RESULT_CODE Compact::isIntersects(ICompact const* const other, bool& result) const
{
  if (isNullptr(other, logger_)) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  if (other->getDim() != getDim()) {
    validLogging(logger_, "Unequal dimensions in isIntersects", RESULT_CODE::WRONG_DIM);
    return RESULT_CODE::WRONG_DIM;
  }

  std::unique_ptr<IVector> otherBegin(other->getBegin()), otherEnd(other->getEnd());

  for (int i = 0; i < begin_->getDim(); ++i) {
    double r = std::min(end_->getCoord(i), otherEnd->getCoord(i)),
           l = std::max(begin_->getCoord(i), otherBegin->getCoord(i));

    if (l > r) {
      result = false;
      return RESULT_CODE::SUCCESS;
    }
  }
  result = true;
  return RESULT_CODE::SUCCESS;
}


size_t Compact::getDim() const
{
  return begin_->getDim();
}


ICompact* Compact::clone() const
{
  return new Compact(begin_->clone(), end_->clone(), logger_);
}



Compact::iterator_impl::iterator_impl(IVector const * const startPoint, 
                                      IVector const * const endPoint,
                                      IVector * step):
  currentPoint_(startPoint->clone()), 
  startPoint_(startPoint),
  endPoint_(endPoint),
  step_(step),
  dir_(nullptr)
{}


RESULT_CODE Compact::iterator_impl::doStep()
{
  return doStepHelper(0);
}


RESULT_CODE Compact::iterator_impl::doStepHelper(size_t dimByPriority)
{
  if (dimByPriority == startPoint_->getDim()) {
    return RESULT_CODE::OUT_OF_BOUNDS;
  }

  size_t dim = dimByPriority;
  if (dir_) {
    dim = dir_->getCoord(dimByPriority);
  }

  // Determine in what direction iterator are moving
  auto isForward = startPoint_->getCoord(0) < endPoint_->getCoord(0);
  double multiplier = isForward ? 1 : -1;

  if (multiplier * currentPoint_->getCoord(dim) +
      multiplier * step_->getCoord(dim) > multiplier * endPoint_->getCoord(dim)) {
    currentPoint_->setCoord(dim, startPoint_->getCoord(dim));
    return doStepHelper(dimByPriority+1);
  }

  currentPoint_->setCoord(dim, currentPoint_->getCoord(dim) + step_->getCoord(dim));
  return RESULT_CODE::SUCCESS;
}


IVector* Compact::iterator_impl::getPoint() const
{
  return currentPoint_->clone();
}


// dir = [1, 2, 0]
// p = [2, 0, 1]
RESULT_CODE Compact::iterator_impl::setDirection(IVector const* const dir)
{
  if (!dir) {
    return RESULT_CODE::BAD_REFERENCE;
  }

  if (dir->getDim() != startPoint_->getDim()) {
    return RESULT_CODE::WRONG_DIM;
  }

  auto sorted = std::make_unique<double[]>(dir->getDim());
  for (size_t i = 0; i < dir->getDim(); ++i) {
    sorted[i] = dir->getCoord(i);
  }

  std::sort(sorted.get(), sorted.get() + dir->getDim());
  for (size_t i = 0; i < dir->getDim(); ++i) {
    if (!areSame(sorted[i], i)) {
      return RESULT_CODE::WRONG_ARGUMENT;
    }
  }

  dir_.reset(dir->clone());
  return RESULT_CODE::SUCCESS;
}

ICompact::~ICompact() {}
