#include "Real.hpp"

Real::Real(Rational *left, Rational *right):
    left_(left), right_(right)
{}

Real *Real::create(Rational *left, Rational *right)
{
    if (!left || !right) {
        return nullptr;
    }
    return new Real(left, right);
}

Real *add(const Real *lhs, const Real *rhs)
{
    return Real::create(add(lhs->getLeft(), rhs->getLeft()), 
                        add(lhs->getRight(), rhs->getRight()));
}

Real *sub(const Real *lhs, const Real *rhs)
{
    return Real::create(sub(lhs->getLeft(), rhs->getLeft()), 
                        sub(lhs->getRight(), rhs->getRight()));
}

Real *mul(const Real *lhs, const Real *rhs)
{
    auto a = RationalUP(mul(lhs->getLeft(), rhs->getLeft())),
         b = RationalUP(mul(lhs->getLeft(), rhs->getRight())),
         c = RationalUP(mul(lhs->getRight(), rhs->getLeft())),
         d = RationalUP(mul(lhs->getRight(), rhs->getRight()));

    Rational *mn = min(min(a.get(), b.get()), min(c.get(), d.get()));
    Rational *mx = max(max(a.get(), b.get()), max(c.get(), d.get()));
    return Real::create(mn->copy(), mx->copy());
}

Real *div(const Real *lhs, const Real *rhs)
{
    auto a = RationalUP(div(lhs->getLeft(), rhs->getLeft())),
         b = RationalUP(div(lhs->getLeft(), rhs->getRight())),
         c = RationalUP(div(lhs->getRight(), rhs->getLeft())),
         d = RationalUP(div(lhs->getRight(), rhs->getRight()));

    Rational *mn = min(min(a.get(), b.get()), min(c.get(), d.get()));
    Rational *mx = max(max(a.get(), b.get()), max(c.get(), d.get()));
    return Real::create(mn->copy(), mx->copy());
}

const Rational *Real::getLeft() const
{
    return left_.get();
}

const Rational *Real::getRight() const
{
    return right_.get();
}


std::ostream &operator<< (std::ostream &out, const Real &r)
{
    return out << '[' << *r.getLeft() << ", " << *r.getRight() << ']';
}

