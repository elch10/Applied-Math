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

  std::unique_ptr<ISet> set(ISet::createSet(logger));
  set->insert(sum.get(), IVector::NORM::NORM_1, 1e-6);
  set->insert(sub.get(), IVector::NORM::NORM_1, 1e-6);
  set->insert(mul.get(), IVector::NORM::NORM_1, 1e-6);

  std::cout << "Dim=" << set->getDim() << "\tSize=" << set->getSize() << std::endl;
  
  return 0;
}
