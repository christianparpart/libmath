///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: printer.h,v 1.5 2002/04/20 06:31:21 cparpart Exp $
//  (This file contains the expression stream printer specific interface)
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
#define libmath_printer_h

#include <math++/visitor.h>

#include <iosfwd>

namespace math {

/**
  * The TPrinter<> visitor class implements stream printing for
  * for the expression tree. Use the static printOn method to print.
  */
template<class T>
class TPrinter : public TNodeVisitor<T> {
private:
    std::ostream& FStream;
    TPrinter(std::ostream& AOutput);

    /// save print takes additionally care about brackets to be printed
    void savePrint(const TNode<T> *ANode, const TNode<T> *AParent);

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

public:
    /**
      * printOn prints given expression, AExpr, to the given output stream AOutput.
      */
    static void printOn(std::ostream& AOutput, const TNode<T> *AExpr);
    
    /**
      * print prints given expression, AExpr, into a std::string and returns 
      * its value. <br>
      * This is just a wrapper method to printon with std::ostringstream, 
      * but often used.
      */
    static std::string print(const TNode<T> *AExpr);
};

} // namespace math

#include <math++/printer.tcc>

#endif
