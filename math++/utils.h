///////////////////////////////////////////////////////////////////////
//  Math Type Library
//	$Id: utils.h,v 1.5 2002/04/23 02:48:38 cparpart Exp $
//  (This file contains the interface to some utility methods)
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
#ifndef libmath_utils_h
#define libmath_utils_h

#include <vector>
#include <utility>
#include <string>

namespace math {

template<class> class TNode;
template<class> class TLibrary;

/**
  * isPrime returns true if ANumber is a prime number, otherwise false
  */
bool isPrime(unsigned ANumber);

/**
  * primeCount returns the number of primes (APrime) in ANumber.
  * If APrime isn't any prime number then it's returns 0 otherwise the count.
  */
unsigned primeCount(unsigned long long ANumber,
                    unsigned long long APrime);

/**
  * calculates the prim factors for given number (ANumber).
  * as result it is returns the number of prim factors in the
  * given numbers and stored in the AResult vector.
  *
  * The result structure is designed as follows:
  *   first value of a pair is the prim factor.
  *   second value of a pair is the number of occurence.
  */
unsigned factorize(unsigned long long ANumber, 
    std::vector<std::pair<unsigned long long, unsigned long long> >& AResult);

/**
  * Factorizes the given number (ANumber) and returns its result
  * as a well formatted string.
  */
std::string factorize(unsigned long long ANumber);

/**
  * Simply returns calculates expression (AExpression) without usage of
  * any library.
  */
template<class T>
T calculate(const std::string& AExpression);

/**
  * Simply returns calculates expression (AExpression).
  */
template<class T>
T calculate(const std::string& AExpression, const TLibrary<T>&);

/**
  * This method derivates given expression, AExpression, ACount times.
  * and returns its result.
  */
template<class T>
TNode<T> *derive(const TNode<T> *AExpression, unsigned ACount = 1);

/**
  * simplifies given expression
  * example: x*x + 2*pi + x*pi = x^2 + pi * (x + 2)
  */
template<class T>
TNode<T> *simplify(const TNode<T> *AExpression);

/**
  * expands an expression.
  * example: 3x^4-2x^2+1 = x*x*x*x + x*x*x*x + x*x*x*x - x*x - x*x + 1
  */
template<class T>
TNode<T> *expand(const TNode<T> *AExpression);

/**
  * clones given expression by calling its clone routine
  */
template<class T>
TNode<T> *copyOf(const TNode<T> *AExpression);

/**
  * Creates an expression tree equivalent to given input.
  */
template<class T>
TNode<T> *createTree(const std::string& AExprStr);

/**
  * Creates the "umkehrfunktion" of given input tree.
  */
template<class T>
TNode<T> *umkehrfunktion(const TNode<T> *ATree);

/**
  * Creates the integral of given input tree.
  */
template<class T>
TNode<T> *integral(const TNode<T> *ATree);

} // namespace math

#include <math++/utils.tcc>

#endif
