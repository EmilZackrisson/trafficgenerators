#ifndef _RNDBI_INCL
#define _RNDBI_INCL

#include <cmath>
#include "rnd.h"

class RNDBI : public RND
{
private:
    double mu1_, sigma1_;
    double mu2_, sigma2_;
    double weightFirst_;
    double secondMaxCut_;

public:
    // Constructor mirrors the Go constructor parameters
    RNDBI(double mu1, double sigma1,
          double mu2, double sigma2,
          int firstCount, int secondCount,
          double secondMaxCut,
          long seed)
        : mu1_(mu1), sigma1_(sigma1),
          mu2_(mu2), sigma2_(sigma2),
          secondMaxCut_(secondMaxCut)
    {
        if (sigma1_ <= 0.0 || sigma2_ <= 0.0)
            throw "sigma must be positive";
        if (firstCount <= 0 || secondCount <= 0)
            throw "counts must be positive";

        weightFirst_ = static_cast<double>(firstCount) /
                       static_cast<double>(firstCount + secondCount);

        SetSeedValue(seed);
    }

    virtual ~RNDBI() {}

    // Returns a double sample (implements RND interface)
    // Box-Muller normal generator using base RND uniform generator
    double Normal()
    {
        double u1 = RND::Rnd();
        double u2 = RND::Rnd();
        if (u1 <= 0.0) u1 = 1e-12;
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    }

    virtual inline double Rnd()
    {
        for (;;)
        {
            if (RND::Rnd() < weightFirst_)
            {
                double v = mu1_ + sigma1_ * Normal();
                if (v > 0.0)
                    return v;
                continue;
            }

            double v = mu2_ + sigma2_ * Normal();
            if (v < secondMaxCut_)
                return v;
        }
    }

    // Returns rounded long sample
    virtual inline long RndL()
    {
        return static_cast<long>(lround(Rnd()));
    }
};

#endif