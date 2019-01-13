/**
* (c) Daniel Lemire, 2008
* (c) Earlence Fernandes, Vrije Universiteit Amsterdam 2011
*
* This C++ library implements dynamic time warping (DTW). 
* This library includes the dynamic programming solution for vectored input signals represented
* by the class TrackPoint. Currently, it has 3 dimensions - x, y, z. More can easily be added to this class.
* No change would be required to the DTW class. Only keep in mind that the distance code has to be updated
* to accomodate more dimensions.
*  
* Time series are represented using STL vectors.
*/

#ifndef VDTW
#define VDTW
#include <iostream>
using namespace std;
#include <cmath>

#define MAX(a,b) ((a)>(b))?(a):(b)
#define MIN(a,b) ((a)<(b))?(a):(b)

typedef unsigned int uint;
//Vector DTW code
class TrackPoint
{
public:
	double x, y, z;
    double acc_dist;

	TrackPoint(double X, double Y, double Z): x(X), y(Y), z(Z) { }

	//euclidean distance
	double euclid_distance(const TrackPoint &p)
	{
		return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
	}
};

class VectorDTW 
{
private:
    std::vector<std::vector<double> > mGamma;
//    double mGamma [200][200];
    int mN, mConstraint;
public:
    enum { INF = 10000 }; //some big number
    
    static inline double min (double x, double y ) { return x < y ? x : y; }

	/**
	* n is the length of the time series 
	*
	* constraint is the maximum warping distance.
	* Typically: constraint = n/10.
	* If you set constraint = n, things will be slower.
	*
	*/
    VectorDTW(uint n, uint constraint) : mGamma(n, std::vector<double>(n, INF)), mN(n), mConstraint(constraint) { }
    
//    VectorDTW(uint n, uint constraint):mN(n), mConstraint(constraint){}
    
    
	/**
	* This currently uses euclidean distance. You can change it to whatever is needed for your application
	*/
    inline double fastdynamic(std::vector<TrackPoint> &v, std::vector<TrackPoint> &w)
    {
            assert(static_cast<int>(v.size()) == mN);
            assert(static_cast<int>(w.size()) == mN);
            assert(static_cast<int>(mGamma.size()) == mN);
            double Best(INF);
            for (int i = 0; i < mN; ++i)
        {
                assert(static_cast<int>(mGamma[i].size()) == mN);
            for(int j = MAX(0, i - mConstraint); j < min(mN, i + mConstraint + 1); ++j)
            {
                        Best = INF;
                        if(i > 0)
                    Best = mGamma[i - 1][j];
                        if(j > 0)
                    Best = min(Best, mGamma[i][j - 1]);
                        if((i > 0) && (j > 0))
                             Best = min(Best, mGamma[i - 1][j - 1]);
                        if((i == 0) && (j == 0))
                              mGamma[i][j] = v[i].euclid_distance(w[j]);
                        else
                              mGamma[i][j] = Best + v[i].euclid_distance(w[j]);
                    }
            }

             return mGamma[mN-1][mN-1];
    }
};
#endif


