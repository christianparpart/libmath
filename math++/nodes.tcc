///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: nodes.tcc,v 1.9 2002/07/02 00:56:12 cparpart Exp $
//  (This file contains the expression tree specific template members)
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

#ifndef libmath_nodes_h
#error You may not include math++/nodes.tcc directly; include math++/nodes.h instead.
#endif

#include <math++/visitor.h>
#include <math++/utils.h>

#include <cmath>

namespace math {

// TNode
template<typename T>
TNode<T>::TNode(TNodeType ANodeType, short APriority, TNode<T> *AParent) :
    FNodeType(ANodeType), FPriority(APriority), FParent(AParent) {
}

template<typename T>
TNode<T>::TNode(const TNode<T>& ANode) :
    FNodeType(ANode.FNodeType),
    FPriority(ANode.FPriority),
    FParent(ANode.FParent) {
}

template<typename T>
void TNode<T>::parent(TNode<T> *AParent) {
    FParent = AParent;
}

template<typename T>
TNode<T>::~TNode() {
}

template<typename T>
typename TNode<T>::TNodeType TNode<T>::nodeType() const {
    return FNodeType;
}

template<typename T>
short TNode<T>::priority() const {
    return FPriority;
}

template<typename T>
TNode<T> *TNode<T>::parent() const {
    return const_cast<TNode<T> *>(FParent);
}

template<typename T>
TNode<T> *TNode<T>::left() const {  
    // the node doesn't support children by default
    return 0;
}

template<typename T>
TNode<T> *TNode<T>::right() const {
    // the node doesn't support children by default
    return 0;
}

template<typename T> bool operator==(const TNode<T>& a, const TNode<T>& b) {
    return a.equals(&b);
}

template<typename T> bool operator!=(const TNode<T>& a, const TNode<T>& b) {
    return !(a == b);
}

// TNumberNode
template<typename T>
TNumberNode<T>::TNumberNode(const T& ANumber) :
    TNode<T>(TNode<T>::NUMBER_NODE, 0), FNumber(ANumber) {
}

template<typename T>
T TNumberNode<T>::number() const {
    return FNumber;
}

template<typename T>
void TNumberNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TNumberNode<T> *TNumberNode<T>::clone() const {
    return new TNumberNode<T>(FNumber);
}

template<typename T>
bool TNumberNode<T>::equals(const TNode<T> *ANode) const {
    return this && ANode && ANode->nodeType() == TNode<T>::NUMBER_NODE
        && FNumber == static_cast<const TNumberNode<T> *>(ANode)->FNumber;
}

// TSymbolNode
template<typename T>
TSymbolNode<T>::TSymbolNode(const std::string& ASymbol) :
    TNode<T>(TNode<T>::SYMBOL_NODE, 0), FSymbol(ASymbol) {
}

template<typename T>
std::string TSymbolNode<T>::symbol() const {
    return FSymbol;
}

template<typename T>
void TSymbolNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TSymbolNode<T> *TSymbolNode<T>::clone() const {
    return new TSymbolNode<T>(FSymbol);
}

template<typename T>
bool TSymbolNode<T>::equals(const TNode<T> *ANode) const {
    return this && ANode && ANode->nodeType() == TNode<T>::SYMBOL_NODE
        && FSymbol == static_cast<const TSymbolNode<T> *>(ANode)->FSymbol;
}

// TParamNode
template<typename T>
TParamNode<T>::TParamNode() :
    TNode<T>(TNode<T>::PARAM_NODE, 0) {
}

template<typename T>
void TParamNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TParamNode<T> *TParamNode<T>::clone() const {
    return new TParamNode<T>();
}

template<typename T>
bool TParamNode<T>::equals(const TNode<T> *ANode) const {
    return this && ANode && ANode->nodeType() == TNode<T>::PARAM_NODE;
}

// TUnaryNodeOp
template<typename T>
TUnaryNodeOp<T>::TUnaryNodeOp(typename TUnaryNodeOp<T>::TNodeType AType, short APrio, TNode<T> *ANode) :
    TNode<T>(AType, APrio), FNode(ANode) {
    
    FNode->parent(this);
}

template<typename T>
TNode<T> *TUnaryNodeOp<T>::node() const {
    return const_cast<TNode<T> *>(FNode.get());
}

template<typename T>
TNode<T> *TUnaryNodeOp<T>::right() const {
    return const_cast<TNode<T> *>(FNode.get());
}

template<typename T>
bool TUnaryNodeOp<T>::equals(const TNode<T> *ANode) const {
    // this method does not make use of the left() for performance reasons

    return this && ANode && this->nodeType() == ANode->nodeType() &&
        FNode->equals(static_cast<const TUnaryNodeOp<T> *>(ANode)->FNode.get());
}

// TBinaryNodeOp
template<typename T>
TBinaryNodeOp<T>::TBinaryNodeOp(typename TBinaryNodeOp<T>::TNodeType AType, short APrio, TNode<T> *ALeft, 
    TNode<T> *ARight) :
    TNode<T>(AType, APrio), FLeft(ALeft), FRight(ARight) {

    FLeft->parent(this);
    FRight->parent(this);
}

template<typename T>
TNode<T> *TBinaryNodeOp<T>::left() const {
    return const_cast<TNode<T> *>(FLeft.get());
}

template<typename T>
TNode<T> *TBinaryNodeOp<T>::right() const {
    return const_cast<TNode<T> *>(FRight.get());
}

template<typename T>
bool TBinaryNodeOp<T>::equals(const TNode<T> *ANode) const {
    // this method does not make use of the left() and right() methods 
    // for performance reasons

    return this && ANode && this->nodeType() == ANode->nodeType() &&
        FLeft->equals(static_cast<const TBinaryNodeOp<T> *>(ANode)->FLeft.get()) &&
        FRight->equals(static_cast<const TBinaryNodeOp<T> *>(ANode)->FRight.get());
}

// TPlusNode
template<typename T>
TPlusNode<T>::TPlusNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::PLUS_NODE, -5, ALeft, ARight) {
}

template<typename T>
void TPlusNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TPlusNode<T> *TPlusNode<T>::clone() const {
    return new TPlusNode(this->left()->clone(), this->right()->clone());
}

// TNegNode
template<typename T>
TNegNode<T>::TNegNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::NEG_NODE, -5, ANode) {
}

template<typename T>
void TNegNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TNegNode<T> *TNegNode<T>::clone() const {
    return new TNegNode(this->node()->clone());
}

// TMulNode
template<typename T>
TMulNode<T>::TMulNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::MUL_NODE, -3, ALeft, ARight) {
}

template<typename T>
void TMulNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TMulNode<T> *TMulNode<T>::clone() const {
    return new TMulNode(this->left()->clone(), this->right()->clone());
}

// TDivNode
template<typename T>
TDivNode<T>::TDivNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::DIV_NODE, -3, ALeft, ARight) {
}

template<typename T>
void TDivNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TDivNode<T> *TDivNode<T>::clone() const {
    return new TDivNode(this->left()->clone(), this->right()->clone());
}

// TPowNode
template<typename T>
TPowNode<T>::TPowNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::POW_NODE, -1, ALeft, ARight) {
}

template<typename T>
void TPowNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TPowNode<T> *TPowNode<T>::clone() const {
    return new TPowNode<T>(this->left()->clone(), this->right()->clone());
}

// TSqrtNode
template<typename T>
TSqrtNode<T>::TSqrtNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::SQRT_NODE, -1, ANode) {
}

template<typename T>
void TSqrtNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TSqrtNode<T> *TSqrtNode<T>::clone() const {
    return new TSqrtNode(this->node()->clone());
}

// TSinNode
template<typename T>
TSinNode<T>::TSinNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::SIN_NODE, -1, ANode) {
}

template<typename T>
void TSinNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TSinNode<T> *TSinNode<T>::clone() const {
    return new TSinNode(this->node()->clone());
}

// TCosNode
template<typename T>
TCosNode<T>::TCosNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::COS_NODE, -1, ANode) {
}

template<typename T>
void TCosNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TCosNode<T> *TCosNode<T>::clone() const {
    return new TCosNode(this->node()->clone());
}

// TTanNode
template<typename T>
TTanNode<T>::TTanNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::TAN_NODE, -1, ANode) {
}

template<typename T>
void TTanNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TTanNode<T> *TTanNode<T>::clone() const {
    return new TTanNode(this->node()->clone());
}

// TCoTanNode
// TArcSinNode
// TArcCosNode
// TArcTanNode
// TArcCoTanNode

// TLnNode
template<typename T>
TLnNode<T>::TLnNode(TNode<T> *ANode) :
    TUnaryNodeOp<T>(TNode<T>::LN_NODE, -1, ANode) {
}

template<typename T>
void TLnNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TLnNode<T> *TLnNode<T>::clone() const {
    return new TLnNode(this->node()->clone());
}

// TFuncNode
template<typename T>
TFuncNode<T>::TFuncNode(const std::string& AName, TNode<T> *AParam) :
    TUnaryNodeOp<T>(TNode<T>::FUNC_NODE, -1, AParam), FName(AName) {
}

template<typename T>
std::string TFuncNode<T>::name() const {
    return FName;
}

template<typename T>
void TFuncNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TFuncNode<T> *TFuncNode<T>::clone() const {
    return new TFuncNode(FName, this->node()->clone());
}

// TIfNode
template<typename T>
TIfNode<T>::TIfNode(TNode<T> *ACondNode, TNode<T> *AThenNode, TNode<T> *AElseNode) :
    TBinaryNodeOp<T>(TNode<T>::IF_NODE, -1, AThenNode, AElseNode),
    FCondition(ACondNode) {
}

template<typename T>
TNode<T> *TIfNode<T>::condition() const {
    return FCondition.get();
}

template<typename T>
TNode<T> *TIfNode<T>::trueExpr() const {
    return this->left();
}

template<typename T>
TNode<T> *TIfNode<T>::falseExpr() const {
    return this->right();
}

template<typename T>
void TIfNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TIfNode<T> *TIfNode<T>::clone() const {
    return new TIfNode(this->FCondition->clone(), this->left()->clone(), this->right()->clone());
}

// TEquNode
template<typename T>
TEquNode<T>::TEquNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::EQU_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TEquNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TEquNode<T> *TEquNode<T>::clone() const {
    return new TEquNode<T>(this->left()->clone(), this->right()->clone());
}

// TUnEquNode
template<typename T>
TUnEquNode<T>::TUnEquNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::UNEQU_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TUnEquNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TUnEquNode<T> *TUnEquNode<T>::clone() const {
    return new TUnEquNode<T>(this->left()->clone(), this->right()->clone());
}

// TGreaterNode
template<typename T>
TGreaterNode<T>::TGreaterNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::GREATER_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TGreaterNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TGreaterNode<T> *TGreaterNode<T>::clone() const {
    return new TGreaterNode<T>(this->left()->clone(), this->right()->clone());
}

// TLessNode
template<typename T>
TLessNode<T>::TLessNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::LESS_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TLessNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TLessNode<T> *TLessNode<T>::clone() const {
    return new TLessNode<T>(this->left()->clone(), this->right()->clone());
}

// TGreaterEquNode
template<typename T>
TGreaterEquNode<T>::TGreaterEquNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::GREATER_EQU_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TGreaterEquNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TGreaterEquNode<T> *TGreaterEquNode<T>::clone() const {
    return new TGreaterEquNode<T>(this->left()->clone(), this->right()->clone());
}

// TLessEquNode
template<typename T>
TLessEquNode<T>::TLessEquNode(TNode<T> *ALeft, TNode<T> *ARight) :
    TBinaryNodeOp<T>(TNode<T>::LESS_EQU_NODE, -10, ALeft, ARight) {
}

template<typename T>
void TLessEquNode<T>::accept(TNodeVisitor<T>& v) {
    v.visit(this);
}

template<typename T>
TLessEquNode<T> *TLessEquNode<T>::clone() const {
    return new TLessEquNode<T>(this->left()->clone(), this->right()->clone());
}

} // namespace math
