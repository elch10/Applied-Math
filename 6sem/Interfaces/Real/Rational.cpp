#include "Rational.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>

int greatestCommonDivision (int a, int b)
{
	while (b) {
		a %= b;
		std::swap (a, b);
	}
	return a;
}

int leastCommonMultiplier(int a, int b)
{
    return a / greatestCommonDivision (a, b) * b;
}

Rational *Rational::create(int num, int denom)
{
    if (denom == 0) {
        std::cerr << "Denom cannot be zero: " << __FILE__ << __LINE__ << std::endl;
        return nullptr;
    }
    return new Rational(num, denom);
}


Rational::Rational(int num, int denom):
    num_(num), denom_(denom)
{
    contract();
    resolveSign();
}

Rational *Rational::copy() const
{
    return create(num_, denom_);
}

void Rational::resolveSign()
{
    bool negative = (num_ < 0) ^ (denom_ < 0);
    num_ = abs(num_);
    if (negative) {
        num_ = -num_;
    }
    denom_ = abs(denom_);
}

void Rational::contract()
{
    int gcd = greatestCommonDivision(num_, denom_);
    num_ /= gcd;
    denom_ /= gcd;
}

int Rational::getNum() const
{
    return num_;
}

int Rational::getDenom() const
{
    return denom_;
}

std::ostream &operator <<(std::ostream &out, const Rational &r)
{
    out << r.getNum();
    if (r.getDenom() != 1) {
        out << '/' << r.getDenom();
    }
    return out;
}

Rational *add(const Rational * lhs, const Rational *rhs)
{
    int lcm = leastCommonMultiplier(lhs->getDenom(), rhs->getDenom());
    int num = lcm / lhs->getDenom() * lhs->getNum() 
            + lcm / rhs->getDenom() * rhs->getNum();
    return Rational::create(num, lcm);
}

Rational *sub(const Rational * lhs, const Rational *rhs)
{
    /*
     * Unique_ptr for auto deleting
     */
    auto temp = RationalUP(Rational::create(-rhs->getNum(), rhs->getDenom()));
    return add(lhs, temp.get());
}

Rational *mul(const Rational * lhs, const Rational *rhs)
{
    return Rational::create(lhs->getNum() * rhs->getNum(), 
                            lhs->getDenom() * rhs->getDenom());
}

Rational *div(const Rational * lhs, const Rational *rhs)
{
    if (rhs->getNum() == 0) {
        return nullptr;
    }
    return mul(lhs, Rational::create(rhs->getDenom(), rhs->getNum()));
}


Rational *min(Rational *lhs, Rational *rhs)
{
    auto diff = RationalUP(sub(lhs, rhs));
    if (diff->getNum() < 0) {
        return lhs;
    }
    return rhs;
}

Rational *max(Rational *lhs, Rational *rhs)
{
    Rational *mn = min(lhs, rhs);
    if (mn != lhs) {
        return lhs;
    }
    return rhs;
}
