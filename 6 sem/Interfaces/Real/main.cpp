#include <iostream>

#include "Rational.hpp"
#include "Real.hpp"

int main()
{
    Rational *r1 = Rational::create(6, 9); // 2/3
    Rational *r2 = Rational::create(-10, 4); // 10/4

    if (!r1 || !r2) {
        return 0;
    }

    RationalUP sum = RationalUP(add(r1, r2)),
               diff = RationalUP(sub(r1, r2)),
               mult = RationalUP(mul(r1, r2)),
               divi = RationalUP(div(r1, r2));

    
    std::cout << "r1=" << *r1 << "\tr2=" << *r2 << std::endl;
    std::cout << "r1+r2=" << *sum << "\tr1-r2=" << *diff 
        << "\tr1*r2=" << *mult << "\tr1/r2=" << *divi  << std::endl;

    std::unique_ptr<Real> r = std::unique_ptr<Real>(Real::create(Rational::create(1,20), 
                                                    Rational::create(2,20))), 
                          rr = std::unique_ptr<Real>(Real::create(r1->copy(), r2->copy()));

    RealUP rsum = RealUP(add(r.get(), rr.get())),
           rdiff = RealUP(sub(r.get(), rr.get())),
           rmul = RealUP(mul(r.get(), rr.get())),
           rdiv = RealUP(div(r.get(), rr.get()));
    std::cout << *r << '\t' << *rr << std::endl
        << *rsum << '\t' << *rdiff << '\t' << *rmul 
        << '\t' << *rdiv << std::endl;
    return 0;
}