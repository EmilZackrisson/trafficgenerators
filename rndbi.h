#ifndef _RNDBI_INCL
#define _RNDBI_INCL

#include <cmath>
#include "rnd.h"

class RNDBI : public RND
{
protected:
    double wSmall;
    double wLarge;

    double smallValue;
    double largeValue;

    double mu;
    double sigma;

    // Box-Muller normal generator
    double Normal()
    {
        double u1 = Rnd();
        double u2 = Rnd();

        return std::sqrt(-2.0 * std::log(u1)) *
               std::cos(2.0 * M_PI * u2);
    }

public:
    RNDBI(double w1, double w2,
          double s1, double s2,
          double m, double s)
        : wSmall(w1),
          wLarge(w2),
          smallValue(s1),
          largeValue(s2),
          mu(m),
          sigma(s)
    {
        if (wSmall + wLarge > 1.0)
            throw "Invalid mixture weights";
    }

    virtual ~RNDBI() {}

    // Returns double sample
    virtual inline double Rnd()
    {
        double u = RND::Rnd();  // base uniform

        if (u < wSmall)
            return smallValue;

        if (u < wSmall + wLarge)
            return largeValue;

        // Lognormal part
        double z = Normal();
        return std::exp(mu + sigma * z);
    }

    // Returns long sample (rounded)
    virtual inline long RndL()
    {
        return (long)(Rnd() + 0.5);
    }
};

#endif