///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: derive.tcc,v 1.7 2002/05/03 11:51:17 cparpart Exp $
//  (This file contains the derivation specific template member methods)
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
namespace math {

template<class T>
TNode<T> *TDeriver<T>::derive(TNode<T> *AExpression) {
    TDeriver<T> derive;

    AExpression->accept(derive);

    return derive.FResult;
}

template<class T>
TDeriver<T>::TDeriver() : FResult(0) {
}

template<class T>
void TDeriver<T>::visit(TNumberNode<T> *ANode) {
    // [const]' = 0

    FResult = new TNumberNode<T>(T(0));
}

template<class T>
void TDeriver<T>::visit(TSymbolNode<T> *ANode) {
    // [const]' = 0

    FResult = new TNumberNode<T>(T(0));
}

template<class T>
void TDeriver<T>::visit(TParamNode<T> *ANode) {
    // [x]' = 1 <:=  [x^1]' = 1x^0
    //           :=>   [x]' = 1

    FResult = new TNumberNode<T>(T(1));
}

template<class T>
void TDeriver<T>::visit(TPlusNode<T> *ANode) {
    // [f + g]' = f' + g'

    FResult = new TPlusNode<T>(
        derive(ANode->left()),
        derive(ANode->right())
    );
}

template<class T>
void TDeriver<T>::visit(TNegNode<T> *ANode) {
    // [-f]' = -(f')

    FResult = new TNegNode<T>(
        derive(ANode->node())
    );
}

template<class T>
void TDeriver<T>::visit(TMulNode<T> *ANode) {
    // [f * g]' = f'g + g'f

    FResult = new TPlusNode<T>(
        new TMulNode<T>(
            derive(ANode->left()),
            ANode->right()->clone()
        ),
        new TMulNode<T>(
            derive(ANode->right()),
            ANode->left()->clone()
        )
    );
}

template<class T>
void TDeriver<T>::visit(TDivNode<T> *ANode) {
    // [f(x)/g(x)]' = (f'g - g'f)/(g^2), g <> 0

    FResult = new TDivNode<T>(
        new TPlusNode<T>(
            new TMulNode<T>(
                derive(ANode->left()),
                ANode->right()->clone()
            ),
            new TNegNode<T>(
                new TMulNode<T>(
                    derive(ANode->right()),
                    ANode->left()->clone()
                )
            )
        ),
        new TPowNode<T>(
            ANode->right()->clone(),
            new TNumberNode<T>(2)
        )
    );
}

template<class T>
void TDeriver<T>::visit(TPowNode<T> *ANode) {
    // [f ^ g]' = f^g * (g'ln(f) + (f'g)/f)

    FResult = new TMulNode<T>(
        ANode->clone(),
        new TPlusNode<T>(
            new TMulNode<T>(
                derive(ANode->right()),
                new TLnNode<T>(ANode->left()->clone())
            ),
            new TDivNode<T>(
                new TMulNode<T>(
                    derive(ANode->left()),
                    ANode->right()->clone()
                ),
                ANode->left()->clone()
            )
        )
    );
}

template<class T>
void TDeriver<T>::visit(TSqrtNode<T> *ANode) {
    // [sqrt(f)]' = sqrt'(f) * f'
}

template<class T>
void TDeriver<T>::visit(TSinNode<T> *ANode) {
    // [sin(f(x))]' = sin'(f(x))*f'(x)
    //              = cos(f(x))*f'(x)
    FResult = new TMulNode<T>(
        new TCosNode<T>(
            ANode->node()->clone()
        ),
        derive(ANode->node())
    );
}

template<class T>
void TDeriver<T>::visit(TCosNode<T> *ANode) {
    // [cos(f)]' = -(sin(f) * f')
    FResult = new TNegNode<T>(
        new TMulNode<T>(
            new TSinNode<T>(
                ANode->node()->clone()
            ),
            derive(ANode->node())
        )
    );
}

template<class T>
void TDeriver<T>::visit(TTanNode<T> *ANode) {
    // [tan(f)]' = (1 + tan(f)^2) * f'
    FResult = new TMulNode<T>(
        new TPlusNode<T>(
            new TNumberNode<T>(T(1)),
            new TPowNode<T>(
                ANode->clone(),
                new TNumberNode<T>(T(2))
            )
        ),
        derive(ANode->node())
    );
}

template<class T>
void TDeriver<T>::visit(TLnNode<T> *ANode) {
    // [ln(f)]' = f' / f
    FResult = new TDivNode<T>(
        derive(ANode->node()),
        ANode->node()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TFuncNode<T> *ANode) {
    // TODO : Implement a good TFuncNode<> derivation here.
    FResult = ANode->clone();
}

template<class T>
void TDeriver<T>::visit(TIfNode<T> *ANode) {
    FResult = new TIfNode<T>(
        ANode->condition()->clone(),
        derive(ANode->trueExpr()), 
        derive(ANode->falseExpr())
    );
}

template<class T>
void TDeriver<T>::visit(TEquNode<T> *ANode) {
    FResult = new TEquNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TUnEquNode<T> *ANode) {
    FResult = new TUnEquNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TGreaterNode<T> *ANode) {
    FResult = new TGreaterNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TLessNode<T> *ANode) {
    FResult = new TLessNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TGreaterEquNode<T> *ANode) {
    FResult = new TGreaterEquNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

template<class T>
void TDeriver<T>::visit(TLessEquNode<T> *ANode) {
    FResult = new TLessEquNode<T>(
        ANode->left()->clone(),
        ANode->right()->clone()
    );
}

} // namespace math
