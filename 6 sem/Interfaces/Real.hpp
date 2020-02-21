#ifndef REAL_HPP
#define REAL_HPP

#include "Rational.hpp"

class Real
{
public:
    Real(const Rational &left, const Rational &right);

    Real &operator+= (const Real &rhs);
    Real &operator-= (const Real &rhs);
    Real &operator*= (const Real &rhs);
    Real &operator/= (const Real &rhs);

    Real operator+ (const Real &rhs) const;
    Real operator- (const Real &rhs) const;
    Real operator* (const Real &rhs) const;
    Real operator/ (const Real &rhs) const;

    Rational getLeft() const;
    Rational getRight() const;

private:
    Rational left_, right_;
};

std::ostream &operator<< (std::ostream &out, const Real &r);

#endif
