#include <iostream>
#include <memory>

#include "ILogger.h"
#include "IVector.h"
#include "ISet.h"
#include "ICompact.h"

using Vect = std::unique_ptr<IVector>;

int main()
{
  size_t dim = 2;
  auto pData1 = std::make_unique<double[]>(dim), 
     pData2 = std::make_unique<double[]>(dim),
     pData3 = std::make_unique<double[]>(dim),
     pData4 = std::make_unique<double[]>(dim),
     stepData = std::make_unique<double[]>(dim),
     dirData = std::make_unique<double[]>(dim);

  for (int i = 0; i < dim; ++i) {
    pData1[i] = i;
    pData2[i] = i + 2;
    pData3[i] = i + 1;
    pData4[i] = i + 3;
    stepData[i] = 1;
    dirData[i] = dim - i - 1;
  }

  ILogger *logger = ILogger::createLogger(nullptr);
  logger->setLogFile("log.txt");

  Vect Vec1(IVector::createVector(dim, pData1.get(), logger)),
    Vec2(IVector::createVector(dim, pData2.get(), logger)),
    Vec3(IVector::createVector(dim, pData3.get(), logger)),
    Vec4(IVector::createVector(dim, pData4.get(), logger)),
    step(IVector::createVector(dim, stepData.get(), logger)),
    dir(IVector::createVector(dim, dirData.get(), logger));

  // Vect sum(IVector::add(Vec1.get(), Vec2.get(), logger)),
  //   sub(IVector::sub(Vec1.get(), Vec2.get(), logger)),
  //   mul(IVector::mul(Vec1.get(), 5, logger));
  
  // double dot = IVector::mul(Vec1.get(), Vec2.get(), logger);

  // std::cout << Vec1.get() << std::endl << Vec2.get() << std::endl;
  // std::cout << "sum=" << sum.get() << "\tsub=" << sub.get() << "\tmul=" << mul.get()
  //       << "\tdot=" << dot << std::endl;

  // std::unique_ptr<ISet> set1(ISet::createSet(logger));
  // set1->insert(sum.get(), IVector::NORM::NORM_1, 1e-6);
  // set1->insert(sub.get(), IVector::NORM::NORM_1, 1e-6);
  // set1->insert(mul.get(), IVector::NORM::NORM_1, 1e-6);

  // std::cout << "Set1: Dim=" << set1->getDim() << "\tSize=" << set1->getSize() << std::endl;
  
  // std::unique_ptr<ISet> set2(ISet::createSet(logger));
  // set2->insert(Vec1.get(), IVector::NORM::NORM_2, 1e-6);
  // set2->insert(sum.get(), IVector::NORM::NORM_2, 1e-6);

  // std::cout << "Set2: Dim=" << set2->getDim() << "\tSize=" << set2->getSize() << std::endl;

  // std::unique_ptr<ISet> symSub(ISet::symSub(set1.get(), set2.get(), IVector::NORM::NORM_1, 1e-6, logger));

  // std::cout << "Symmetric difference set: Dim=" << symSub->getDim() << "\tSize=" << symSub->getSize() << std::endl;



  std::unique_ptr<ICompact> comp(ICompact::createCompact(Vec1.get(), Vec2.get(), logger));

  std::unique_ptr<ICompact::iterator> iter(comp->begin(step.get()));
  iter->setDirection(dir.get());

  RESULT_CODE rc = RESULT_CODE::SUCCESS;
  do {
    std::unique_ptr<IVector> point(iter->getPoint());
    std::cout << point.get() << std::endl;
    rc = iter->doStep();
  } while (rc == RESULT_CODE::SUCCESS);

  
  std::unique_ptr<ICompact> comp2(ICompact::createCompact(Vec3.get(), Vec4.get(), logger));

  bool isIntersects = false;
  comp2->isIntersects(comp.get(), isIntersects);
  std::cout << "Is intersects: " << isIntersects << std::endl;

  std::unique_ptr<ICompact> comp3(ICompact::add(comp.get(), comp2.get(), logger));
  std::cout << "Add returns: " << comp3.get() << std::endl;

  std::unique_ptr<ICompact> comp4(ICompact::makeConvex(comp.get(), comp2.get(), logger));

  iter.reset(comp4->begin(step.get()));
  iter->setDirection(dir.get());

  rc = RESULT_CODE::SUCCESS;
  do {
    std::unique_ptr<IVector> point(iter->getPoint());
    std::cout << point.get() << std::endl;
    rc = iter->doStep();
  } while (rc == RESULT_CODE::SUCCESS);

  return 0;
}
