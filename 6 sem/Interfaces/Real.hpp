#ifndef REAL_HPP
#define REAL_HPP

#include "Rational.hpp"

/*
 * Represents real number.
 * It is represented as segment beetwen two rational numbers
 */
class Real
{
public:

    const Rational *getLeft() const;
    const Rational *getRight() const;

    /*
     * It doesn't deep copy. It moves resources. 
     */
    static Real *create(Rational *left, Rational *right);
private:
    Real(Rational *left, Rational *right);
    RationalUP left_, right_;
};

using RealUP = std::unique_ptr<Real>;

std::ostream &operator<< (std::ostream &out, const Real &r);

Real *add(const Real *lhs, const Real *rhs);
Real *sub(const Real *lhs, const Real *rhs);
Real *mul(const Real *lhs, const Real *rhs);
Real *div(const Real *lhs, const Real *rhs);

#endif
