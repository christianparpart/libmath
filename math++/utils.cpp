///////////////////////////////////////////////////////////////////////
//  Math Type Library
//	$Id: utils.cpp,v 1.2 2002/04/08 21:09:12 cparpart Exp $
//  (This file contains the methods to some utility methods)
//
//  Copyright (c) 2002 by Christian Parpart <cparpart@surakware.net>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
// 
//  You should have received a copy of the GNU Library General Public License
//  along with this library; see the file COPYING.LIB.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.
///////////////////////////////////////////////////////////////////////
#include <math++/utils.h>
#include <sstream>

#include <cmath>

namespace math {

inline unsigned long long pow(unsigned long long ABase, unsigned long long AExp) {
    unsigned long long result = 1;

    while (AExp--)
        result *= ABase; 

    return result;
}

bool isPrime(unsigned long long ANumber) {
    if (ANumber == 1)
        return false;

    if (!(ANumber & 1))
        return ANumber == 2;

    if (ANumber == 3)
        return true;

    unsigned long long n = 3;
    while (ANumber % n) {
        n += 2;
        
        if ((n * n) > ANumber)
            return true;
    }

    return false;
}

unsigned primeCount(unsigned long long ANumber,
                              unsigned long long APrime) {
    if ((!(ANumber & 1) && ANumber != 2) || !isPrime(ANumber))
        return 0;

    unsigned long long exp = 0;
    while (!(ANumber % pow(APrime, exp++)));
    return exp - 2;
}

unsigned factorize(unsigned long long ANumber,
    std::vector<std::pair<unsigned long long, unsigned long long> >& AResult) {

    AResult.erase(AResult.begin(), AResult.end());

    for (unsigned long long b = 2; b < ANumber; ++b)
        if (unsigned long long e = primeCount(ANumber, b))
            AResult.push_back(std::make_pair(b, e));

    if (AResult.empty())
        AResult.push_back(std::make_pair(ANumber, 1));

    return AResult.size();
}

// converts given number to string
inline std::string strcnv(unsigned long long ANumber) {
    std::stringstream s;
    s << ANumber;
    return s.str();
}

std::string factorize(unsigned long long ANumber) {
    std::string result;

    for (unsigned long long b = 2; b < ANumber; ++b) {
        if (unsigned long long e = primeCount(ANumber, b)) {
            if (result.size())
                result += "*";

            result += std::string(strcnv(b))
                    + std::string("^")
                    + std::string(strcnv(e));
        }
    }

    if (result.empty())
        result = std::string(strcnv(ANumber))
               + std::string("^1");

    return result;
}

} // namespace math
