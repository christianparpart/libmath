///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: printer.tcc,v 1.7 2002/05/05 10:48:24 cparpart Exp $
//  (This file contains the expression stream printer specific template members)
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
#ifndef libmath_printer_h
#error You may not include math++/printer.tcc directly; include math++/printer.h instead.
#endif

#include <sstream>

namespace math {

template<class T>
TPrinter<T>::TPrinter(std::ostream& AOutput) :
    FStream(AOutput) {
}

template<class T>
void TPrinter<T>::printOn(std::ostream& AOutput, const TNode<T> *ANode) {
    TPrinter<T> printer(AOutput);
    const_cast<TNode<T> *>(ANode)->accept(printer);
}

template<class T>
std::string TPrinter<T>::print(const TNode<T> *ANode) {
    std::stringstream sstr;
    TPrinter<T> printer(sstr);
    const_cast<TNode<T> *>(ANode)->accept(printer);
    sstr << std::ends;

    return sstr.str();
}

template<class T>
void TPrinter<T>::savePrint(const TNode<T> *ANode, const TNode<T> *AParent) {
#if 1  // ifndef __ENABLE_DEBUG 
    // just disabled for debugging to see reproduce the node order
    bool less = ANode->priority() < AParent->priority() ||
               (AParent->nodeType() == TNode<T>::NEG_NODE &&
                 (ANode->nodeType() == TNode<T>::PLUS_NODE ||
                  ANode->nodeType() == TNode<T>::NEG_NODE));
#else
    bool less = true;//ANode->priority() < AParent->priority();
#endif
    if (less)
        FStream << "(";

    const_cast<TNode<T> *>(ANode)->accept(*this);

    if (less)
        FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TNumberNode<T> *ANode) {
    FStream << ANode->number();
}

template<class T>
void TPrinter<T>::visit(TSymbolNode<T> *ANode) {
    FStream << ANode->symbol();
}

template<class T>
void TPrinter<T>::visit(TParamNode<T> *ANode) {
    FStream << "x"; // the parameter is usually x,
                    // so keep this symbol reserved, or override me;)
}

template<class T>
void TPrinter<T>::visit(TPlusNode<T> *ANode) {
    savePrint(ANode->left(), ANode);
    if (ANode->right()->nodeType() != TNode<T>::NEG_NODE)
        FStream << "+";
    savePrint(ANode->right(), ANode);
}

template<class T>
void TPrinter<T>::visit(TNegNode<T> *ANode) {
    FStream << "-";
    savePrint(ANode->node(), ANode);
}

template<class T>
void TPrinter<T>::visit(TMulNode<T> *ANode) {
    savePrint(ANode->left(), ANode);

    // print 2x isteat of 2*x
    // print 2(x+y) instead of 2*(x+y)
    // print (a+b)(c+d) instead of (a+b)*(c+d)
    //if (!(ANode->left()->nodeType() == TNode<T>::NUMBER_NODE &&
    //      (ANode->right()->nodeType() == TNode<T>::SYMBOL_NODE ||
    //       ANode->right()->nodeType() == TNode<T>::PARAM_NODE)))
        FStream << "*";

    savePrint(ANode->right(), ANode);
}

template<class T>
void TPrinter<T>::visit(TDivNode<T> *ANode) {
    savePrint(ANode->left(), ANode);
    FStream << "/";
    savePrint(ANode->right(), ANode);
}

template<class T>
void TPrinter<T>::visit(TPowNode<T> *ANode) {
    savePrint(ANode->left(), ANode);
    FStream << "^";
    savePrint(ANode->right(), ANode);
}

template<class T>
void TPrinter<T>::visit(TSqrtNode<T> *ANode) {
    FStream << "sqrt(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TSinNode<T> *ANode) {
    FStream << "sin(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TCosNode<T> *ANode) {
    FStream << "cos(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TTanNode<T> *ANode) {
    FStream << "tan(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TLnNode<T> *ANode) {
    FStream << "ln(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TFuncNode<T> *ANode) {
    FStream << ANode->name() << "(";
    ANode->node()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TIfNode<T> *ANode) {
    FStream << "IF(";
    ANode->condition()->accept(*this);
    FStream << ", ";
    ANode->trueExpr()->accept(*this);
    FStream << ", ";
    ANode->falseExpr()->accept(*this);
    FStream << ")";
}

template<class T>
void TPrinter<T>::visit(TEquNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << "=";
    ANode->right()->accept(*this);
}

template<class T>
void TPrinter<T>::visit(TUnEquNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << "<>";
    ANode->right()->accept(*this);
}

template<class T>
void TPrinter<T>::visit(TGreaterNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << ">";
    ANode->right()->accept(*this);
}

template<class T>
void TPrinter<T>::visit(TLessNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << "<";
    ANode->right()->accept(*this);
}

template<class T>
void TPrinter<T>::visit(TGreaterEquNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << ">=";
    ANode->right()->accept(*this);
}

template<class T>
void TPrinter<T>::visit(TLessEquNode<T> *ANode) {
    ANode->left()->accept(*this);
    FStream << "<=";
    ANode->right()->accept(*this);
}

/*template<class T>
std::ostream& operator<< <T>(std::ostream& os, const TNode<T>& ANode) {
    TPrinter<T>::print(os, &ANode);
    return os;
}*/

} // namespace math
