#include "Rational.hpp"
#include <cmath>
#include <stdexcept>

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

Rational::Rational(int num):
    Rational(num, 1)
{}


Rational::Rational(int num, int denom):
    num_(num), denom_(denom)
{
    if (denom == 0) {
        throw std::invalid_argument("Denominator cannot be zero");
    }
    contract();
    resolveSign();
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


Rational &Rational::operator+= (const Rational &rhs)
{
    int lcm = leastCommonMultiplier(denom_, rhs.denom_);
    num_ *= lcm / denom_;
    num_ += lcm / rhs.denom_ * rhs.num_;
    denom_ = lcm;
    contract();
    resolveSign();
    return *this;
}

Rational &Rational::operator-= (const Rational &rhs)
{
    Rational cpy = rhs;
    cpy.num_ *= -1;
    *this += cpy;
    return *this;
}

Rational &Rational::operator*= (const Rational &rhs)
{
    num_ *= rhs.num_;
    denom_ *= rhs.denom_;
    contract();
    resolveSign();
    return *this;
}

Rational &Rational::operator/= (const Rational &rhs)
{
    if (rhs.num_ == 0) {
        throw std::invalid_argument("Cannot divide to zero");
    }
    Rational cpy = rhs;
    std::swap(cpy.num_, cpy.denom_);
    cpy.resolveSign();
    *this *= cpy;
    return *this;
}


Rational Rational::operator+ (const Rational &rhs) const
{
    Rational cpy = *this;
    cpy += rhs;
    return cpy;
}

Rational Rational::operator- (const Rational &rhs) const
{
    Rational cpy = *this;
    cpy -= rhs;
    return cpy;
}

Rational Rational::operator* (const Rational &rhs) const
{
    Rational cpy = *this;
    cpy *= rhs;
    return cpy;
}

Rational Rational::operator/ (const Rational &rhs) const
{
    Rational cpy = *this;
    cpy /= rhs;
    return cpy;
}

bool Rational::operator< (const Rational &rhs) const
{
    return (*this - rhs).num_ < 0;
}

bool Rational::operator> (const Rational &rhs) const
{
    return (*this - rhs).num_ > 0;
}

bool Rational::operator== (const Rational &rhs) const
{
    return !(*this > rhs) && !(*this < rhs);
}

bool Rational::operator<= (const Rational &rhs) const
{
    return *this < rhs || *this == rhs;
}

bool Rational::operator>= (const Rational &rhs) const
{
    return *this > rhs || *this == rhs;
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

