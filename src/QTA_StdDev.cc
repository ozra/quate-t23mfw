#include "QTA_StdDev.hh"
#ifdef INTERFACE
/**
* Created:  2014-11-29
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include "QTABase.hh"

namespace QTA {


/*

*TODO* - this is a stub!

*/

/*

std::pair<double,double> getMeanVariance(const std::vector<double>& vec) {
    double mean = 0, M2 = 0;

    size_t n = vec.size();
    for(size_t i=0; i<n; ++i) {
        double delta = vec[i] - mean;
        mean += delta/n;
        M2 += delta*(vec[i] - mean);
        variance = M2/(n - 1)
        // <-- You can use the running mean and variance here
    }

    std_dev = sqrt(variance);


    return std::make_pair(mean,variance);
}
*/

}

#endif
