///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: calculator.tcc,v 1.1 2002/04/20 06:39:18 cparpart Exp $
//  (implements the function calculator)
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
#error You may not include math++/calculator.tcc directly; include math++/calculator.h instead.
#endif

#include <cmath>

namespace math {

template<class T>
T TCalculator<T>::calculate(const TFunction<T>& AFunction, const T& AParam,
    const TLibrary<T>& ALibrary, unsigned ALimit) {

    TCalculator<T> c(AFunction, AParam, ALibrary, ALimit);
    return c.FResult;
}

template<class T>
TCalculator<T>::TCalculator(const TFunction<T>& AFunction, const T& AParam, 
    const TLibrary<T>& ALibrary, unsigned ALimit) :
    FParam(AParam), FLibrary(ALibrary), FLimit(ALimit) {

    AFunction.expression()->accept(*this);
}

template<class T>
T TCalculator<T>::calculate(const TNode<T> *AExpression) {
    const_cast<TNode<T> *>(AExpression)->accept(*this);

    return FResult;
}

template<class T>
void TCalculator<T>::visit(TNumberNode<T> *ANode) {
    FResult = ANode->number();
}

template<class T>
void TCalculator<T>::visit(TSymbolNode<T> *ANode) {
    FResult = FLibrary.value(ANode->symbol());
}

template<class T>
void TCalculator<T>::visit(TParamNode<T> *ANode) {
    FResult = FParam;
}

template<class T>
void TCalculator<T>::visit(TPlusNode<T> *ANode) {
    FResult = calculate(ANode->left()) + calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TNegNode<T> *ANode) {
    FResult = - calculate(ANode->node());
}

template<class T>
void TCalculator<T>::visit(TMulNode<T> *ANode) {
    FResult = calculate(ANode->left()) * calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TDivNode<T> *ANode) {
    FResult = calculate(ANode->left()) / calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TPowNode<T> *ANode) {
    FResult = pow(calculate(ANode->left()), calculate(ANode->right()));
}

template<class T>
void TCalculator<T>::visit(TSqrtNode<T> *ANode) {
    FResult = sqrt(calculate(ANode->node()));
}

template<class T>
void TCalculator<T>::visit(TSinNode<T> *ANode) {
    FResult = sin(calculate(ANode->node()));
}

template<class T>
void TCalculator<T>::visit(TCosNode<T> *ANode) {
    FResult = cos(calculate(ANode->node()));
}

template<class T>
void TCalculator<T>::visit(TTanNode<T> *ANode) {
    FResult = tan(calculate(ANode->node()));
}

template<class T>
void TCalculator<T>::visit(TLnNode<T> *ANode) {
    FResult = log(calculate(ANode->node()));
}

template<class T>
void TCalculator<T>::visit(TFuncNode<T> *ANode) {
    const std::string name(ANode->name());

    if (FRecursions.find(name) == FRecursions.end())
        FRecursions[name] = 0;

    if (++FRecursions[name] > FLimit)
        throw ECalcError("Function exceeds recursion counter: " + name + ".");

    T save(FParam);
    FParam = calculate(ANode->node());

    FResult = calculate(FLibrary.function(name).expression());

    FParam = save;
}

template<class T>
void TCalculator<T>::visit(TIfNode<T> *ANode) {
    FResult = calculate(ANode->condition())
            ? calculate(ANode->trueExpr())
            : calculate(ANode->falseExpr());
}

template<class T>
void TCalculator<T>::visit(TEquNode<T> *ANode) {
    FResult = calculate(ANode->left()) == calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TUnEquNode<T> *ANode) {
    FResult = calculate(ANode->left()) != calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TGreaterNode<T> *ANode) {
    FResult = calculate(ANode->left()) > calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TLessNode<T> *ANode) {
    FResult = calculate(ANode->left()) < calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TGreaterEquNode<T> *ANode) {
    FResult = calculate(ANode->left()) >= calculate(ANode->right());
}

template<class T>
void TCalculator<T>::visit(TLessEquNode<T> *ANode) {
    FResult = calculate(ANode->left()) <= calculate(ANode->right());
}

} // namespace math
