#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <sstream>

class Rational
{
public:
    Rational(int num);
    Rational(int num, int denom);

    Rational &operator+= (const Rational &rhs);
    Rational &operator-= (const Rational &rhs);
    Rational &operator*= (const Rational &rhs);
    Rational &operator/= (const Rational &rhs);

    Rational operator+ (const Rational &rhs) const;
    Rational operator- (const Rational &rhs) const;
    Rational operator* (const Rational &rhs) const;
    Rational operator/ (const Rational &rhs) const;

    bool operator< (const Rational &rhs) const;
    bool operator> (const Rational &rhs) const;
    bool operator== (const Rational &rhs) const;

    bool operator<= (const Rational &rhs) const;
    bool operator>= (const Rational &rhs) const;
    
    int getNum() const;
    int getDenom() const;
private:
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


int greatestCommonDivision (int a, int b);
int leastCommonMultiplier (int a, int b);

std::ostream &operator<< (std::ostream &out, const Rational &r);


#endif
