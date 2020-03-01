#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <sstream>
#include <memory>

class Rational
{
public:
    int getNum() const;
    int getDenom() const;
    Rational *copy() const;

    static Rational *create(int num, int denom);
private:
    Rational(int num, int denom);
    Rational(const Rational &rhs) = delete;
    Rational &operator=(const Rational &rhs) = delete;

    /*
     * Makes sure, that sign is stored only in the numerator
     */
    void resolveSign();

    /*
     * Contracts rational number to irreducible fraction
     */
    void contract();

    int num_, denom_;
};

using RationalUP = std::unique_ptr<Rational>;

std::ostream &operator<< (std::ostream &out, const Rational &r);


int greatestCommonDivision (int a, int b);
int leastCommonMultiplier (int a, int b);

/*
 * This 4 functions create new element. Do not forget to free them.
 */
Rational *add(const Rational *lhs, const Rational *rhs);
Rational *sub(const Rational *lhs, const Rational *rhs);
Rational *mul(const Rational *lhs, const Rational *rhs);
Rational *div(const Rational *lhs, const Rational *rhs);


Rational *min(Rational *lhs, Rational *rhs);
Rational *max(Rational *lhs, Rational *rhs);

#endif
