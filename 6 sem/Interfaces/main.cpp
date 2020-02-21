#include <iostream>

#include "Rational.hpp"
#include "Real.hpp"

int main()
{
    Rational r1(6, 9); // 2/3
    Rational r2(-10, 4); // 10/4

    
    std::cout << "r1=" << r1 << "\tr2=" << r2 << std::endl;
    std::cout << "r1+r2=" << r1+r2 << "\tr1-r2=" << r1-r2 
        << "\tr1*r2=" << r1*r2 << "\tr1/r2=" << r1/r2  << std::endl;

    Real r(Rational(1,20), Rational(2,20)), rr(r1, r2);
    std::cout << r << '\t' << rr << std::endl
        << r + rr << '\t' << r - rr << '\t' << r * rr 
        << '\t' << r / rr << std::endl;
    return 0;
}