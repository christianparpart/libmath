///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: simplifier.h,v 1.6 2002/04/29 19:34:30 cparpart Exp $
//  (This file contains the expression simplifier interface)
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
#define libmath_simplifier_h

#include <math++/visitor.h>

namespace math {

/**
  * \todo: complete the simplifier implementation (using expr pattern matching).
  */
template<typename T>
class TSimplifier : public TNodeVisitor<T> {
public:
    /// Simplifies given expression and returns its result.
    static TNode<T> *simplify(const TNode<T> *AExpression);

private:
    TNode<T> *FResult;

private:
    TSimplifier();

    /// calculate() is used to calculate partial expressions to check its static value.
    T calculate(const TNode<T> *) const;

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

#include <math++/simplifier.tcc>

#endif
