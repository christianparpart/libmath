///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: simplifier.tcc,v 1.9 2002/05/05 10:49:09 cparpart Exp $
//  (This file contains the expression simplifier template members)
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
#ifndef libmath_simplifier_h
#error You may not include math++/simplify.tcc directly; include math++/simplify.h instead.
#endif

#include <math++/library.h>
#include <math++/calculator.h>

#include <memory>

namespace math {

template<class T>
bool isConst(const TNode<T> *ANode) {
    // note, that symbols may not be interpreted as const here, even if they're
    return !ANode 
        || (ANode->nodeType() != TNode<T>::PARAM_NODE
            && ANode->nodeType() != TNode<T>::SYMBOL_NODE
            && ANode->nodeType() < TNode<T>::FUNC_NODE
            && isConst(ANode->left()) && isConst(ANode->right()));
}

template<class T>
TNode<T> *TSimplifier<T>::simplify(const TNode<T> *AExpression) {
    TNode<T> *oldResult = 0;
    TNode<T> *newResult = AExpression->clone();

    do {
        delete oldResult;
        oldResult = newResult;

        TSimplifier<T> simplifier;
        oldResult->accept(simplifier);

        newResult = simplifier.FResult;
    } while (*newResult != *oldResult);

    delete oldResult;

    return newResult;
}

template<class T>
TSimplifier<T>::TSimplifier() :
    FResult(0) {
}

template<class T>
T TSimplifier<T>::calculate(const TNode<T> *AExpr) const {
    static TLibrary<T> library;

    if (!library.constants()) {
        library.insert(TConstant<T>("pi", 3.1415));// M_PI)); // just in case they're used. :)
        library.insert(TConstant<T>("e", 2.1718));// M_E));
    }

    return TCalculator<T>::calculate(TFunction<T>("tmp", AExpr), T(), library);
}

template<class T>
void TSimplifier<T>::visit(TNumberNode<T> *ANode) {
    // (-number) = -(number)
    T value(ANode->number());
    if (value < 0) {
        FResult = new TNegNode<T>(
            new TNumberNode<T>(-value)
        );
        return;
    }

    FResult = ANode->clone();
}

template<class T>
void TSimplifier<T>::visit(TSymbolNode<T> *ANode) {
    FResult = ANode->clone();
}

template<class T>
void TSimplifier<T>::visit(TParamNode<T> *ANode) {
    FResult = ANode->clone();
}

template<class T>
void TSimplifier<T>::visit(TPlusNode<T> *ANode) {
    std::auto_ptr<TNode<T> > left(simplify(ANode->left()));
    std::auto_ptr<TNode<T> > right(simplify(ANode->right()));

    // check constness
    if (isConst(left.get()) && isConst(right.get())) {
        // Do not use simple calculate(ANode):
        FResult = new TNumberNode<T>(calculate(left.get()) + calculate(right.get()));
        return;
    }

    // 0+a = a
    if (left->nodeType() == TNode<T>::NUMBER_NODE && calculate(left.get()) == T(0)) {
        FResult = right->clone();
        return;
    }

    // a+0 = a
    if (right->nodeType() == TNode<T>::NUMBER_NODE && calculate(right.get()) == T(0)) {
        FResult = left->clone();
        return;
    }

    // a+(-0) = a
    if (right->nodeType() == TNode<T>::NEG_NODE && 
            right->right()->nodeType() == TNode<T>::NUMBER_NODE &&
            calculate(right->right()) == T(0)) {
        FResult = left->clone();
        return;
    }

    // a + a = 2a
    if (*left == *right) {
        FResult = new TMulNode<T>(
            new TNumberNode<T>(T(2)),
            right->clone()
        );
        return;
    }

    // a + (-a) = 0
    if (right->nodeType() == TNode<T>::NEG_NODE && *left == *right->right()) {
        FResult = new TNumberNode<T>(T(0));
        return;
    }

    // (a + b) + (-b) = b
    if (left->nodeType() == TNode<T>::PLUS_NODE && right->nodeType() == TNode<T>::NEG_NODE
            && *left->right() == *right->right()) {
        FResult = left->left()->clone();
        return;
    }

    // a + -(a + b) = -b
    if (right->nodeType() == TNode<T>::NEG_NODE 
            && right->right()->nodeType() == TNode<T>::PLUS_NODE
            && *left == *right->right()->left()) {
        FResult = new TNegNode<T>(
            right->right()->right()->clone()
        );
        return;
    }

    // (-a) + (-b) = -(a + b)
    if (left->nodeType() == TNode<T>::NEG_NODE) {
        if (right->nodeType() == TNode<T>::NEG_NODE) {
            FResult = new TNegNode<T>(
                new TPlusNode<T>(
                    left->right()->clone(),
                    right->right()->clone()
                )
            );
        } else {
            // (-a) + b = b + (-a)              FORM TRANSFORMATION
            FResult = new TPlusNode<T>(
                right->clone(),
                left->clone()
            );
        }
        return;
    }

    // n * a + a = a(n + 1)
    if (left->nodeType() == TNode<T>::MUL_NODE && *left->right() == *right) {
        FResult = new TMulNode<T>(
            right->clone(),
            new TPlusNode<T>(
                left->left()->clone(),
                new TNumberNode<T>(T(1))
            )
        );
        return;
    }

    // a*b + a*c = a(b + c)
    if (left->nodeType() == TNode<T>::MUL_NODE &&
            right->nodeType() == TNode<T>::MUL_NODE && 
            *left->left() == *right->left()) {
        FResult = new TMulNode<T>(
            left->left()->clone(),
            new TPlusNode<T>(
                left->right()->clone(),
                right->right()->clone()
            )
        );
        return;
    }
    // a*b + a*c = a*b + c*a = b*a + a*c = b*a + c*a = a(b + c)
    // WE MUST FIND AN ALGORITHM FOR GENERIC PATTERN MATCHING SOON !!!

    // (f * g) + (f / h) =  f * (g + 1/h)
    if (left->nodeType() == TNode<T>::MUL_NODE 
            && right->nodeType() == TNode<T>::DIV_NODE
            && *left->left() == *right->left()) {
        FResult = new TMulNode<T>(
            left->left()->clone(),
            new TPlusNode<T>(
                left->right()->clone(),
                new TDivNode<T>(
                    new TNumberNode<T>(T(1)),
                    right->right()->clone()
                )
            )
        );
        return;
    }

    // nothing special found, just optimized left and right child nodes
    FResult = new TPlusNode<T>(left->clone(), right->clone());
}

template<class T>
void TSimplifier<T>::visit(TNegNode<T> *ANode) {
    std::auto_ptr<TNode<T> > node(simplify(ANode->node()));
    
    // -(-a) = a
    if (node->nodeType() == TNode<T>::NEG_NODE) {
        FResult = node->right()->clone();
        return;
    }

    // nothing special found, just optimized left and right child nodes
    FResult = new TNegNode<T>(node->clone());
}

template<class T>
void TSimplifier<T>::visit(TMulNode<T> *ANode) {
    std::auto_ptr<TNode<T> > left(simplify(ANode->left()));
    std::auto_ptr<TNode<T> > right(simplify(ANode->right()));

    // check constness
    if (isConst(left.get()) && isConst(right.get())) {
        FResult = new TNumberNode<T>(calculate(ANode));
        return;
    }

    // 0*a = 0
    if (left->nodeType() == TNode<T>::NUMBER_NODE && calculate(left.get()) == T(0)) {
        FResult = new TNumberNode<T>(T(0));
        return;
    }

    // a*0 = 0
    if (right->nodeType() == TNode<T>::NUMBER_NODE && calculate(right.get()) == T()) {
        FResult = new TNumberNode<T>(T(0));
        return;
    }

    // 1*a = a, a 
    if (left->nodeType() == TNode<T>::NUMBER_NODE && calculate(left.get()) == T(1)) {
        FResult = right->clone();
        return;
    }

    // a*1 = a, a 
    if (right->nodeType() == TNode<T>::NUMBER_NODE && calculate(right.get()) == T(1)) {
        FResult = left->clone();
        return;
    }

    // a*a = a^2
    if (*left == *right) {
        FResult = new TPowNode<T>(left->clone(), new TNumberNode<T>(T(2)));
        return;
    }

    // (-a) * b = -(a * b)
    if (left->nodeType() == TNode<T>::NEG_NODE) {
        FResult = new TNegNode<T>(
            new TMulNode<T>(
                left->right()->clone(),
                right->clone()
            )
        );
        return;
    }
    
    // a * (-b) = -(a * b)
    if (right->nodeType() == TNode<T>::NEG_NODE) {
        FResult = new TNegNode<T>(
            new TMulNode<T>(
                left->clone(),
                right->right()->clone()
            )
        );
        return;
    }

    // a^n*a = a^(n+1)
    if (left->nodeType() == TNode<T>::POW_NODE && *left->left() == *right) {
        FResult = new TPowNode<T>(
            right->clone(),
            new TPlusNode<T>(
                left->right()->clone(),
                new TNumberNode<T>(T(1))
            )
        );
        return;
    }

    // (C*a)*D = (C*D)*a; C, D const.
    if (left->nodeType() == TNode<T>::MUL_NODE
            && isConst(left->left()) && isConst(right.get())) {
        FResult = new TMulNode<T>(
            new TNumberNode<T>(
                T(calculate(left->left()) * calculate(right.get()))
            ),
            left->right()->clone()
        );
        return;
    }

    // (a*b)*b = a*b^2
    if (left->nodeType() == TNode<T>::MUL_NODE
            && *left->right() == *right.get()) {
        FResult = new TMulNode<T>(
            left->left()->clone(),
            new TPowNode<T>(
                right->clone(),
                new TNumberNode<T>(T(2))
            )
        );
        return;
    }

    // (a*b)*a = b*a^2
    if (left->nodeType() == TNode<T>::MUL_NODE
            && *left->left() == *right.get()) {
        FResult = new TMulNode<T>(
            left->right()->clone(),
            new TPowNode<T>(
                right->clone(),
                new TNumberNode<T>(T(2))
            )
        );
        return;
    }

    // (a * b) * b^c = a * b^(c + 1)
    if (left->nodeType() == TNode<T>::MUL_NODE 
            && right->nodeType() == TNode<T>::POW_NODE
            && *left->right() == *right->left()) {
        FResult = new TMulNode<T>(
            left->left()->clone(),
            new TPowNode<T>(
                left->right()->clone(),
                new TPlusNode<T>(
                    right->right()->clone(),
                    new TNumberNode<T>(T(1))
                )
            )
        );
        return;
    }        

    // a^b*c/a = c*a^(b - 1)
    if (left->nodeType() == TNode<T>::POW_NODE 
            && right->nodeType() == TNode<T>::DIV_NODE && 
            *left->left() == *right->right()) {

        FResult = new TMulNode<T>(
            right->left()->clone(),
            new TPowNode<T>(
                left->left()->clone(),
                new TPlusNode<T>(
                    left->right()->clone(),
                    new TNegNode<T>(new TNumberNode<T>(T(1)))
                )
            )
        );
        return;
    }

    // (a/b)*c = (a*c)/b
    if (left->nodeType() == TNode<T>::DIV_NODE) {
        FResult = new TDivNode<T>(
            new TMulNode<T>(
                left->left()->clone(),
                right->clone()
            ),
            left->right()->clone()
        );
        return;
    }

    // a * a^b = a^(b+1)
    if (right->nodeType() == TNode<T>::POW_NODE &&
            *left == *right->left()) {
        FResult = new TPowNode<T>(
            left->clone(),
            new TPlusNode<T>(
                right->right()->clone(),
                new TNumberNode<T>(T(1))
            )
        );
        return;
    }

    // nothing special found, just optimized left and right child nodes
    FResult = new TMulNode<T>(left->clone(), right->clone());
}

template<class T>
void TSimplifier<T>::visit(TDivNode<T> *ANode) {
    std::auto_ptr<TNode<T> > left(simplify(ANode->left()));
    std::auto_ptr<TNode<T> > right(simplify(ANode->right()));

    // check constness
    if (isConst(left.get()) && isConst(right.get())) {
        T divisor(calculate(right.get()));
        if (divisor == T(0))
            // prevent division by zero, by no simplifying
            FResult = new TDivNode<T>(left->clone(), right->clone());
        else
            FResult = new TNumberNode<T>(calculate(left.get()) / divisor);

        return;
    }

    // 0/a = 0
    if (left->nodeType() == TNode<T>::NUMBER_NODE && calculate(left.get()) == T(0)) {
        FResult = new TNumberNode<T>(T(0));
        return;
    }

    // a / a = 1
    if (*left == *right) {
        FResult = new TNumberNode<T>(T(1));
        return;
    }

    // (-a) / b = -(a / b)              FORM TRANSFORMATION
    if (left->nodeType() == TNode<T>::NEG_NODE) {
        FResult = new TNegNode<T>(
            new TDivNode<T>(
                left->right()->clone(),
                right->clone()
            )
        );
        return;
    }

    // (a * b) / c = a * (b / c)        FORM TRANSFORMATION
    if (left->nodeType() == TNode<T>::MUL_NODE) {
        FResult = new TMulNode<T>(
            left->left()->clone(),
            new TDivNode<T>(
                left->right()->clone(),
                right->clone()
            )
        );
        return;
    }

    // a / (-b) = -(a / b)              FORM TRANSFORMATION
    if (right->nodeType() == TNode<T>::NEG_NODE) {
        FResult = new TNegNode<T>(
            new TDivNode<T>(
                left->clone(),
                right->right()->clone()
            )
        );
        return;
    }

    // a / b^c = a * b^(-c)
    if (right->nodeType() == TNode<T>::POW_NODE) {
        FResult = new TMulNode<T>(
            left->clone(),
            new TPowNode<T>(
                right->left()->clone(),
                new TNegNode<T>(
                    right->right()->clone()
                )
            )
        );
        return;
    }

    // (a^b)/a = a^(b-1)
    if (left->nodeType() == TNode<T>::POW_NODE && *left->left() == *right.get()) {
        FResult = new TPowNode<T>(
            right->clone(),
            new TPlusNode<T>(
                left->right()->clone(),
                new TNegNode<T>(new TNumberNode<T>(T(1)))
            )
        );
        return;
    }

    // a^b / a^c = a^(b-c)
    if (left->nodeType() == TNode<T>::POW_NODE && right->nodeType() == TNode<T>::POW_NODE
            && *left->left() == *right->left()) {
        FResult = new TPowNode<T>(
            left->left()->clone(),
            new TPlusNode<T>(
                left->right()->clone(),
                new TNegNode<T>(right->right()->clone())
            )
        );
        return;
    }

    // nothing special found, just optimized left and right child nodes
    FResult = new TDivNode<T>(left->clone(), right->clone());
}

template<class T>
void TSimplifier<T>::visit(TPowNode<T> *ANode) {
    std::auto_ptr<TNode<T> > left(simplify(ANode->left()));
    std::auto_ptr<TNode<T> > right(simplify(ANode->right()));

    // check constness
    if (isConst(left.get()) && isConst(right.get())) {
        FResult = new TNumberNode<T>(calculate(ANode));
        return;
    }

    // a^0 = 1
    if (right->nodeType() == TNode<T>::NUMBER_NODE && calculate(right.get()) == 0) {
        FResult = new TNumberNode<T>(1);
        return;
    }

    // a^1 = a
    if (right->nodeType() == TNode<T>::NUMBER_NODE && calculate(right.get()) == 1) {
        FResult = left->clone();
        return;
    }

    // f^g^h = f^(g * h)
    if (left->nodeType() == TNode<T>::POW_NODE) {
        FResult = new TPowNode<T>(
            left->left()->clone(),
            new TMulNode<T>(
                left->right()->clone(),
                right->clone()
            )
        );
        return;
    }

    // nothing special found, just optimized left and right child nodes
    FResult = new TPowNode<T>(left->clone(), right->clone());
}

template<class T>
void TSimplifier<T>::visit(TSqrtNode<T> *ANode) {
    FResult = ANode->clone();
}

template<class T>
void TSimplifier<T>::visit(TSinNode<T> *ANode) {
    FResult = new TSinNode<T>(simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TCosNode<T> *ANode) {
    FResult = new TCosNode<T>(simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TTanNode<T> *ANode) {
    FResult = new TTanNode<T>(simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TLnNode<T> *ANode) {
    std::auto_ptr<TNode<T> > node(simplify(ANode->node()));

    // ln(e) = 1
    if (node->nodeType() == TNode<T>::SYMBOL_NODE &&
            static_cast<TSymbolNode<T> *>(node.get())->symbol() == "e") {
        FResult = new TNumberNode<T>(T(1));
        return;
    }

    FResult = new TLnNode<T>(node->clone());
}

template<class T>
void TSimplifier<T>::visit(TFuncNode<T> *ANode) {
    FResult = new TFuncNode<T>(ANode->name(), simplify(ANode->node()));
}

template<class T>
void TSimplifier<T>::visit(TIfNode<T> *ANode) {
    FResult = new TIfNode<T>(simplify(ANode->condition()),
        simplify(ANode->trueExpr()), simplify(ANode->falseExpr()));
}

template<class T>
void TSimplifier<T>::visit(TEquNode<T> *ANode) {
    FResult = new TEquNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TUnEquNode<T> *ANode) {
    FResult = new TUnEquNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TGreaterNode<T> *ANode) {
    FResult = new TGreaterNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TLessNode<T> *ANode) {
    FResult = new TLessNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TGreaterEquNode<T> *ANode) {
    FResult = new TGreaterEquNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

template<class T>
void TSimplifier<T>::visit(TLessEquNode<T> *ANode) {
    FResult = new TLessEquNode<T>(simplify(ANode->left()), 
        simplify(ANode->right()));
}

} // namespace math

