//
//  vectordtw_02.h
//  bracelet_log
//
//  Created by 李鹏智 on 2018/10/8.
//  Copyright © 2018年 李鹏智. All rights reserved.
//

#ifndef vectordtw_02_h
#define vectordtw_02_h

/**
 * (c) Daniel Lemire, 2008
 * (c) Earlence Fernandes, Vrije Universiteit Amsterdam 2011
 *
 * This C++ library implements dynamic time warping (DTW).
 * This library includes the dynamic programming solution for vectored input signals represented
 * by the class Point. Currently, it has 3 dimensions - x, y, z. More can easily be added to this class.
 * No change would be required to the DTW class. Only keep in mind that the distance code has to be updated
 * to accomodate more dimensions.
 *
 * Time series are represented using STL vectors.
 */

#ifndef VDTW
#define VDTW

#include <vector>
#include <cmath>
#include <assert.h>

typedef unsigned int uint;
using namespace std;

//Vector DTW code
class Point_02
{
public:
    double x, y, z;
    
    Point(double X, double Y, double Z): x(X), y(Y), z(Z) { }
    
    //computes the l1 distance with another point
    double l1_distance(const Point &p)
    {
        return fabs(x - p.x) + fabs(y - p.y) + fabs(z - p.z);
    }
    
    //euclidean distance
    double euclid_distance(const Point &p)
    {
        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }
};

class VectorDTW_02
{
private:
    vector<vector<double> > mGamma;
    int mN, mConstraint;
public:
    enum { INF = 100000000 }; //some big number
    
    static inline double min (double x, double y ) { return x < y ? x : y; }
    
    /**
     * n is the length of the time series
     *
     * constraint is the maximum warping distance.
     * Typically: constraint = n/10.
     * If you set constraint = n, things will be slower.
     *
     */
    VectorDTW_02(uint n, uint constraint) : mGamma(n, vector<double>(n, INF)), mN(n), mConstraint(constraint) { }
    
    /**
     * This currently uses euclidean distance. You can change it to whatever is needed for your application
     */
    inline double fastdynamic_02(vector<Point> &v, vector<Point> &w)
    {
        assert(static_cast<int>(v.size()) == mN);
        assert(static_cast<int>(w.size()) == mN);
        assert(static_cast<int>(mGamma.size()) == mN);
        double Best(INF);
        for (int i = 0; i < mN; ++i)
        {
            assert(static_cast<int>(mGamma[i].size()) == mN);
            for(int j = max(0, i - mConstraint); j < min(mN, i + mConstraint + 1); ++j)
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
#endif /* vectordtw_02_h */
