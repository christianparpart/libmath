///////////////////////////////////////////////////////////////////////
//  Math Type Library
//	$Id: visitor.h,v 1.4 2002/04/29 19:34:30 cparpart Exp $
//  (This file contains the abstract interface to the expression tree visitor)
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
#ifndef libmath_visitor_h
#define libmath_visitor_h

namespace math {

template<class> class TNumberNode;  // numbers
template<class> class TSymbolNode;  // symbols ("e", "pi", "t", ...")
template<class> class TParamNode;   // usually the symbol "x"

template<class> class TPlusNode;    // operations 1st degree
template<class> class TNegNode;

template<class> class TMulNode;     // operations 2nd degree
template<class> class TDivNode;

template<class> class TPowNode;     // operations 3rd degree

template<class> class TSqrtNode;    // build-in functions
template<class> class TSinNode;
template<class> class TCosNode;
template<class> class TTanNode;
template<class> class TCoTanNode;
template<class> class TLnNode;

template<class> class TFuncNode;    // user defined functions

template<class> class TIfNode;      // extended functions

template<class> class TEquNode;     // equations
template<class> class TUnEquNode;
template<class> class TGreaterNode;
template<class> class TLessNode;
template<class> class TGreaterEquNode;
template<class> class TLessEquNode;

/**
  * TNodeVisitor<> is the abstract base class for the expression tree.
  * It is used to extend the tree by additional functionalities such as
  * derivation, simplifying, or what ever you'd like to add.
  */
template<typename T>
class TNodeVisitor {
public:
    virtual ~TNodeVisitor() {};

    virtual void visit(TNumberNode<T> *) = 0;
    virtual void visit(TSymbolNode<T> *) = 0;
    virtual void visit(TParamNode<T> *) = 0;

    virtual void visit(TPlusNode<T> *) = 0;
    virtual void visit(TNegNode<T> *) = 0;

    virtual void visit(TMulNode<T> *) = 0;
    virtual void visit(TDivNode<T> *) = 0;
    
    virtual void visit(TPowNode<T> *) = 0;
    virtual void visit(TSqrtNode<T> *) = 0;

    virtual void visit(TSinNode<T> *) = 0;
    virtual void visit(TCosNode<T> *) = 0;
    virtual void visit(TTanNode<T> *) = 0;
    virtual void visit(TLnNode<T> *) = 0;

    virtual void visit(TFuncNode<T> *) = 0;
    virtual void visit(TIfNode<T> *) = 0;

    virtual void visit(TEquNode<T> *) = 0;
    virtual void visit(TUnEquNode<T> *) = 0;
    virtual void visit(TGreaterNode<T> *) = 0;
    virtual void visit(TLessNode<T> *) = 0;
    virtual void visit(TGreaterEquNode<T> *) = 0;
    virtual void visit(TLessEquNode<T> *) = 0;
};

} // namespace math

#endif
