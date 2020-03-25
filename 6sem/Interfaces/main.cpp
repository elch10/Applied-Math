#include <iostream>
#include <memory>
#include "IVector.h"
#include "ISet.h"
#include "ILogger.h"

using VectUnique = std::unique_ptr<IVector>;

int main()
{
  size_t dim = 10;
  auto pData1 = std::make_unique<double[]>(dim), 
     pData2 = std::make_unique<double[]>(dim);

  for (int i = 0; i < dim; ++i) {
    pData1[i] = i;
    pData2[i] = i * 10;
  }

  ILogger *logger = ILogger::createLogger(nullptr);
  logger->setLogFile("log.txt");

  VectUnique Vec1(IVector::createVector(dim, pData1.get(), logger)),
    Vec2(IVector::createVector(dim, pData2.get(), logger));

  VectUnique sum(IVector::add(Vec1.get(), Vec2.get(), logger)),
    sub(IVector::sub(Vec1.get(), Vec2.get(), logger)),
    mul(IVector::mul(Vec1.get(), 5, logger));
  
  double dot = IVector::mul(Vec1.get(), Vec2.get(), logger);

  std::cout << Vec1.get() << std::endl << Vec2.get() << std::endl;
  std::cout << "sum=" << sum.get() << "\tsub=" << sub.get() << "\tmul=" << mul.get()
        << "\tdot=" << dot << std::endl;

  std::unique_ptr<ISet> set1(ISet::createSet(logger));
  set1->insert(sum.get(), IVector::NORM::NORM_1, 1e-6);
  set1->insert(sub.get(), IVector::NORM::NORM_1, 1e-6);
  set1->insert(mul.get(), IVector::NORM::NORM_1, 1e-6);

  std::cout << "Set1: Dim=" << set1->getDim() << "\tSize=" << set1->getSize() << std::endl;
  
  std::unique_ptr<ISet> set2(ISet::createSet(logger));
  set2->insert(Vec1.get(), IVector::NORM::NORM_2, 1e-6);
  set2->insert(sum.get(), IVector::NORM::NORM_2, 1e-6);

  std::cout << "Set2: Dim=" << set2->getDim() << "\tSize=" << set2->getSize() << std::endl;

  std::unique_ptr<ISet> symSub(ISet::symSub(set1.get(), set2.get(), IVector::NORM::NORM_1, 1e-6, logger));

  std::cout << "Symmetric difference set: Dim=" << symSub->getDim() << "\tSize=" << symSub->getSize() << std::endl;

  return 0;
}
