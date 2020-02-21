#include "Real.hpp"

Real::Real(const Rational &left, const Rational &right):
    left_(left), right_(right)
{}


Real &Real::operator+= (const Real &rhs)
{
    left_ += rhs.left_;
    right_ += rhs.right_;
    return *this;
}

Real &Real::operator-= (const Real &rhs)
{
    left_ -= rhs.left_;
    right_ -= rhs.right_;
    return *this;
}

Real &Real::operator*= (const Real &rhs)
{
    left_ = std::min(std::min(left_ * rhs.left_, left_ * rhs.right_), 
                std::min(right_ * rhs.left_, right_ * rhs.right_));
    right_ = std::max(std::max(left_ * rhs.left_, left_ * rhs.right_), 
                std::max(right_ * rhs.left_, right_ * rhs.right_));
    return *this;
}

Real &Real::operator/= (const Real &rhs)
{
    left_ = std::min(std::min(left_ / rhs.left_, left_ / rhs.right_), 
                std::min(right_ / rhs.left_, right_ / rhs.right_));
    right_ = std::max(std::max(left_ / rhs.left_, left_ / rhs.right_), 
                std::max(right_ / rhs.left_, right_ / rhs.right_));
    return *this;
}


Real Real::operator+ (const Real &rhs) const
{
    Real cpy = *this;
    cpy += rhs;
    return cpy;
}

Real Real::operator- (const Real &rhs) const
{
    Real cpy = *this;
    cpy -= rhs;
    return cpy;
}

Real Real::operator* (const Real &rhs) const
{
    Real cpy = *this;
    cpy *= rhs;
    return cpy;
}

Real Real::operator/ (const Real &rhs) const
{
    Real cpy = *this;
    cpy /= rhs;
    return cpy;
}


Rational Real::getLeft() const
{
    return left_;
}

Rational Real::getRight() const
{
    return right_;
}


std::ostream &operator<< (std::ostream &out, const Real &r)
{
    return out << '[' << r.getLeft() << ", " << r.getRight() << ']';
}

