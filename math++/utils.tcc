///////////////////////////////////////////////////////////////////////
//  Math Type Library
//	$Id: utils.tcc,v 1.6 2002/05/03 11:51:42 cparpart Exp $
//  (This file contains the template method implementations of the utility methods)
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
#error You may not include math++/utils.tcc directly; use math++/utils.h instead.
#endif

#include <math++/derive.h>
#include <math++/printer.h>
#include <math++/simplifier.h>
#include <math++/library.h>
#include <math++/calculator.h>
#include <math++/reader.h>

#include <memory>

namespace math {

template<class T>
T calculate(const std::string& AExprStr, const TLibrary<T>& ALib) {
    return TCalculator<T>::calculate(TFunction<T>("f", AExprStr), T(), ALib);
}

template<class T>
T calculate(const std::string& AExpression) {
    TLibrary<T> library;
    return calculate<T>(AExpression, library);
}

template<class T>
TNode<T> *derive(const TNode<T> *AExpression, unsigned ACount) {
    TNode<T> *result = AExpression->clone();

    while (--ACount != unsigned(-1)) {
        TNode<T> *r1 = TDeriver<T>::derive(result);
        delete result;

        TNode<T> *r2 = TSimplifier<T>::simplify(r1);
        delete r1;

        result = r2;
    }

    return result;
}

template<class T>
TNode<T> *createTree(const std::string& AExprStr) {
    return TReader<T>::parse(AExprStr);
}

template<class T>
TNode<T> *umkehrfunktion(const TNode<T> *AExprTree) {
    // TODO
    return AExprTree->clone();
}

template<class T>
TNode<T> *integral(const TNode<T> *AExprTree) {
    // TODO
    return AExprTree->clone();
}

}
