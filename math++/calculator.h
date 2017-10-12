///////////////////////////////////////////////////////////////////////
//  Math Type Library
//	$Id: calculator.h,v 1.1 2002/04/20 06:39:18 cparpart Exp $
//  (defines the interface for the function calculator)
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
#ifndef libmath_calculator_h
#define libmath_calculator_h

#include <math++/visitor.h>
#include <math++/error.h>

#include <string>
#include <map>

namespace math {

template<class> class TFunction;
template<class> class TLibrary;

/**
  * ECalcError gets thrown whenever any error during function calculation
  * occurs. This may be e.g. a recursion error.
  */
class ECalcError : public EMath {
public:
    ECalcError(const std::string& AReason) : EMath(AReason) {}
};

/**
  * TCalculator calculates functions results using given function and 
  * a library to use. You may also specify the recursion limit.
  */
template<class T>
class TCalculator : protected TNodeVisitor<T> {
public:
    /// calculates the functions result using given values.
    static T calculate(const TFunction<T>& AFunction, const T& AParam, 
        const TLibrary<T>& ALibrary, unsigned ARecursionLimit = 64);

private:
    T FParam;
    const TLibrary<T>& FLibrary;
    std::map<std::string, unsigned> FRecursions;
    unsigned FLimit;
    T FResult;

private:
    /// creates a calculator
    TCalculator(const TFunction<T>& AFunction, const T& AParam, 
        const TLibrary<T>& ALibrary, unsigned ALimit);

    /// calculates partial expression
    T calculate(const TNode<T> *AExpression);

    virtual void visit(TNumberNode<T> *);
    virtual void visit(TSymbolNode<T> *);
    virtual void visit(TParamNode<T> *);

    virtual void visit(TPlusNode<T> *);
    virtual void visit(TNegNode<T> *);

    virtual void visit(TMulNode<T> *);
    virtual void visit(TDivNode<T> *);

    virtual void visit(TPowNode<T> *);
    virtual void visit(TSqrtNode<T> *);

    virtual void visit(TSinNode<T> *);
    virtual void visit(TCosNode<T> *);
    virtual void visit(TTanNode<T> *);
    virtual void visit(TLnNode<T> *);

    virtual void visit(TFuncNode<T> *);
    virtual void visit(TIfNode<T> *);

    virtual void visit(TEquNode<T> *);
    virtual void visit(TUnEquNode<T> *);
    virtual void visit(TGreaterNode<T> *);
    virtual void visit(TLessNode<T> *);
    virtual void visit(TGreaterEquNode<T> *);
    virtual void visit(TLessEquNode<T> *);
};

} // namespace math

#include <math++/calculator.tcc>

#endif
