#include <iostream>
#include <memory>
#include "IVector.hpp"

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

    auto Vec1 = VectUnique(IVector::createVector(dim, pData1.get())),
         Vec2 = VectUnique(IVector::createVector(dim, pData2.get()));

    auto sum = VectUnique(IVector::add(Vec1.get(), Vec2.get())),
         sub = VectUnique(IVector::sub(Vec1.get(), Vec2.get())),
         mul = VectUnique(IVector::mul(Vec1.get(), 5));
    
    double dot = IVector::mul(Vec1.get(), Vec2.get());

    std::cout << Vec1.get() << std::endl << Vec2.get() << std::endl;
    std::cout << "sum=" << sum.get() << "\tsub=" << sub.get() << "\tmul=" << mul.get()
              << "\tdot=" << dot << std::endl;
    return 0;
}
