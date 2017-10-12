///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: nodes.h,v 1.13 2002/07/02 00:56:12 cparpart Exp $
//  (This file contains the expression tree specific interface)
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
#define libmath_nodes_h

#include <deque>
#include <string>
#include <memory>

/* EXAMPLE
 * expression: 3x^4 + 2x^2 + 1
 * the tree:
 *                 ____ + ____
 *                /           \
 *           ___ + ___         1
 *          /         \
 *         *           *
 *        / \         / \
 *       3   ^       2   ^
 *          / \         / \
 *         x   4       x   2
 */

namespace math {

template<typename> class TNodeVisitor;
template<typename> class TNode;
template<typename> class TUnaryNodeOp;
template<typename> class TBinaryNodeOp;

// ISSUE : we should, perhaps, support two iterator types
//  - one for the real iteration (each and every node, classic),
//  - and one wich takes care about the math privileges and stays 
//    only in its scope

/**
  * TNodeIterator<T> is the implementation of the node iteration class.
  * This is used (as a typedef to TNode<T>::iterator and 
  * TNode<T>::const_iterator) to iterate through the node operations, such as
  * plus, minus, and so on.
  * Use these as you would do with the STL.
  */
template<typename T>
class TNodeIterator {
private:
    TNode<T> *FCurrent;

private:
    void increment() {
        // TODO : it must care about the operator privileges
        if (FCurrent->right()) {
            FCurrent = FCurrent->right();

            while (FCurrent->left())
                FCurrent = FCurrent->left();
        } else {
            TNode<T> *p = FCurrent->parent();

            while (FCurrent == p->right()) {
                FCurrent = p;
                p = p->parent();
            }

            if (FCurrent->right() != p)
                FCurrent = p;
        }
    }

    bool decrement() {
        // TODO : it must care about the operator privileges
        TNode<T> *last = FCurrent;

        if (FCurrent->left()) {
            TNode<T> *p = FCurrent->left();

            while (p->right())
                p = p->right();

            FCurrent = p;
        } else {
            TNode<T> *p = FCurrent->parent();

            while (FCurrent == p->left()) {
                FCurrent = p;
                p = p->left();
            }

            FCurrent = p;
        }
        return FCurrent != last;
    }

    /// returns to the very first node of this level
    TNodeIterator<T>& rewind() {
        while (decrement());

        return *this;
    }
    
    friend class TNode<T>; // TNode<T> may access the method rewind.

public:
    TNodeIterator() : FCurrent(0) {}
    TNodeIterator(TNode<T> *ANode) : FCurrent(ANode) {}
    TNodeIterator(const TNodeIterator<T>& v) : FCurrent(v.FCurrent) {}

    TNode<T>& operator*() const { return *FCurrent; }
    TNode<T> *operator->() const { return FCurrent; }

    TNode<T> *get() const { return FCurrent; }

    TNodeIterator<T>& operator++() { increment(); return *this; }
    TNodeIterator<T>& operator--() { decrement(); return *this; }
};

template<typename T>
bool operator==(const TNodeIterator<T>& a, const TNodeIterator<T>& b) {
    return a.get() == b.get();
}

template<typename T>
bool operator!=(const TNodeIterator<T>& a, const TNodeIterator<T>& b) {
    return a.get() != b.get();
}

/** TOperandIter<> (use TNode<>::operand_iterator and const_operand_iterator)
  * are designed to iterate the operands of an given expression. 
  * (e.g. all summands of an plus operation)
  */
template <typename NodeType>
class TOperandIter {
public:
    TOperandIter() : FOrigin(0), FCurrent(0) {}

    TOperandIter(NodeType *AOperator) : FOrigin(AOperator), FCurrent(FOrigin) {
        do FCurrent = FCurrent->left();
        while (inScope(FCurrent));
    }

    TOperandIter(const TOperandIter<NodeType>& AProto) :
        FOrigin(AProto.FOrigin), FCurrent(AProto.FCurrent) {}

    NodeType& operator*() const { return *FCurrent; }
    NodeType *operator->() const { return FCurrent; }

    NodeType *get() const { return this ? FCurrent : 0; }

    TOperandIter<NodeType>& end() const { return *static_cast<TOperandIter<NodeType> *>(0); }

    TOperandIter<NodeType>& operator++() { increment(); return *this; }

private:
    NodeType *FOrigin;
    NodeType *FCurrent;

    /// find operand right next to the current one and set
    void increment() {
        if (FCurrent) {
            NodeType *p = FCurrent->parent();

            // as long as we're the right child of p
            while (p && FCurrent == p->right() && inScope(p)) {
                // go one up
                FCurrent = p;
                p = p->parent();
            }
            FCurrent = p;

            // reached end of iteration
            if (!p || !inScope(p)) {
                FCurrent = 0;
                return;
            }

            FCurrent = FCurrent->right();
            if (inScope(FCurrent)) {
                do FCurrent = FCurrent->left();
                while (inScope(FCurrent));
            }
        }
    }

    bool inScope(const NodeType *ANode) const {
        switch (FOrigin->nodeType()) {
            case NodeType::PLUS_NODE:
            case NodeType::MUL_NODE:
                return ANode->nodeType() == FOrigin->nodeType();
            default:
                return false;
        }
    }
};

template<typename T>
bool operator==(const TOperandIter<T>& a, const TOperandIter<T>& b) {
    return a.get() == b.get();
}

template<typename T>
bool operator!=(const TOperandIter<T>& a, const TOperandIter<T>& b) {
    return a.get() != b.get();
}

/**
  * TNode<T> represents the abstract node base.
  */
template <typename T>
class TNode {
public:
    typedef TNodeIterator<T> iterator;
    typedef const TNodeIterator<T> const_iterator;

    typedef TOperandIter<TNode<T> > operand_iterator;
    typedef TOperandIter<const TNode<T> > const_operand_iterator;

    /**
      * This enumeration, TNodeType, is implemented for node type access.
      * This makes it also faster because it replaces the dynamic_cast<> operator.
      */
    enum TNodeType {
        // take care, the order below is hard coded

        NUMBER_NODE,    // numbers: 0, 3.1415, 2.17, 0.815, ... (prio: 0)
        SYMBOL_NODE,    // any symbol value: pi, e, ...         (prio: 0)
        PARAM_NODE,     // the function parameter... (e.g. x)   (prio: 0)

        PLUS_NODE,      // x + y                    (prio: -5)
        NEG_NODE,       // -x                       (prio: -5)

        MUL_NODE,       // x * y                    (prio: -3)
        DIV_NODE,       // x / y                    (prio: -3)
        MOD_NODE,       // x mod y                  (prio: -3)

        POW_NODE,       // x ^ y                    (prio: -1)

        EQU_NODE,       // x == y                   (prio: -10)
        UNEQU_NODE,     // x != y                   (prio: -10)
        LESS_EQU_NODE,  // x <= y                   (prio: -10)
        GREATER_EQU_NODE,// x >= y                  (prio: -10)
        LESS_NODE,      // x < y                    (prio: -10)
        GREATER_NODE,   // x > y                    (prio: -10)

        FUNC_NODE,      // userfunc(x)              (prio: -1)

        SQRT_NODE,      // sqrt(x); x ^ 0.5         (prio: -1)
        SIN_NODE,       // sin(x)                   (prio: -1)
        COS_NODE,       // cos(x)                   (prio: -1)
        TAN_NODE,       // tan(x)                   (prio: -1)
        LN_NODE,        // logn(x)                  (prio: -1)

        IF_NODE         // IF(cond, then, else)     (prio: -1)
    };

private:
    /// represents the type of node (used to workaround the slow dynamic_cast<>)
    TNodeType FNodeType;
    /// 0 is highes node priority (the higher the lower)
    short FPriority;
    /// contains a pointer to the parent node if any exists, otherwise 0
    TNode<T> *FParent;

protected:
    /// initializes this node for given node type.
    TNode(TNodeType ANodeType, short APriority, TNode<T> *AParentNode = 0);
    TNode(const TNode<T>& n);

    /// initializes the parent node with the given one
    void parent(TNode<T> *AParent);

    friend class TUnaryNodeOp<T>;
    friend class TBinaryNodeOp<T>;

public:
    /// each virtual class needs a virtual destructor (this one does nothing)
    virtual ~TNode();

    /// returns the type of this node
    TNodeType nodeType() const;

    /// returns the node priority
    short priority() const;

    /// returns the parent node (returns 0 if this node is the root node)
    TNode<T> *parent() const;

    /// returns the left child node (returns 0 if this node doesn't support one)
    virtual TNode<T> *left() const;

    /// returns the right child node (returns 0 if this node doesn't support one)
    virtual TNode<T> *right() const;

    /// calls the visit method in TNodeVisitor<>
    virtual void accept(TNodeVisitor<T>&) = 0;

    /// clones that node
    virtual TNode<T> *clone() const = 0;

    /// iterator access to the first value node in this operator level
    iterator begin() { return iterator(this).rewind(); }

    /// iterator access to the end
    iterator end() { return iterator(0); }

    /// returns true, if given node equals to this one
    virtual bool equals(const TNode<T> *ANode) const = 0;
};

template<typename T> bool operator==(const TNode<T>&, const TNode<T>&);
template<typename T> bool operator!=(const TNode<T>&, const TNode<T>&);

/**
  * TNumberNode<T> represents the node holding a constant value of type T.
  */
template<typename T>
class TNumberNode : public TNode<T> {
private:
    T FNumber;

public:
    TNumberNode(const T& AValue);

    T number() const;

    virtual void accept(TNodeVisitor<T>&);
    virtual TNumberNode<T> *clone() const;
    virtual bool equals(const TNode<T> *ANode) const;
};

/**
  * TSymbolNode<> represents any symbol used in an expression, except
  * the function parameter and build-in functions itself.
  */
template<typename T>
class TSymbolNode : public TNode<T> {
private:
    std::string FSymbol;

public:
    TSymbolNode(const std::string& ASymbol);

    /// returns the symbol's name
    std::string symbol() const;

    virtual void accept(TNodeVisitor<T>&);
    virtual TSymbolNode<T> *clone() const;
    virtual bool equals(const TNode<T> *ANode) const;
};

/**
  * This node represents a parameter node on wich the
  * whole expression tree usualle depends on.
  */
template<typename T>
class TParamNode : public TNode<T> {
public:
    TParamNode();

    virtual void accept(TNodeVisitor<T>&);
    virtual TParamNode<T> *clone() const;
    virtual bool equals(const TNode<T> *ANode) const;
};

/**
  * TUnaryNodeOp<T> represents the base class for any unary operation node
  * in the expression tree.
  */
template<typename T>
class TUnaryNodeOp : public TNode<T> {
private:
    std::auto_ptr<TNode<T> > FNode;

protected:
    /// creates an unary operator node of type AType.
    TUnaryNodeOp(typename TUnaryNodeOp<T>::TNodeType AType, short APriority, TNode<T> *ANode);

public:
    /// returns the child node for that unary operator node.
    TNode<T> *node() const;

    /// returns the child node (wrapper to the more declarative node() method)
    virtual TNode<T> *right() const;

    virtual bool equals(const TNode<T> *ANode) const;
};

/**
  * TBinaryNodeOp<T> represents the base class for any binary operation node
  * in the expression tree.
  */
template<typename T>
class TBinaryNodeOp : public TNode<T> {
private:
    std::auto_ptr<TNode<T> > FLeft;
    std::auto_ptr<TNode<T> > FRight;

protected:
    /// creates an binary operator node of type AType
    TBinaryNodeOp(typename TBinaryNodeOp<T>::TNodeType AType, short APrio, TNode<T> *ALeft, TNode<T> *ARight);

public:
    /// returns the right child node of the expression tree
    virtual TNode<T> *left() const;

    /// returns the left child node of the expression tree
    virtual TNode<T> *right() const;

    virtual bool equals(const TNode<T> *ANode) const;
};

/**
  * The class TPlusNode<T> implements the plus operator
  * for numbers of type T.
  */
template <typename T>
class TPlusNode : public TBinaryNodeOp<T> {
public:
    TPlusNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TPlusNode<T> *clone() const;
};

/** The class TNegNode<T> implements the negotiation operation
  * for numbers of type T
  *
  */
template<typename T>
class TNegNode : public TUnaryNodeOp<T> {
public:
    TNegNode(TNode<T> *ANode);

    virtual void accept(TNodeVisitor<T>&);
    virtual TNegNode<T> *clone() const;
};

/**
  * The class TMulNode<T> implements the muliplication operator
  * for numbers of type T.
  */
template<typename T>
class TMulNode : public TBinaryNodeOp<T> {
public:
    TMulNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TMulNode *clone() const;
};

/**
  * TDivNode implements the division operator for
  * numbers of type T.
  */
template<typename T>
class TDivNode : public TBinaryNodeOp<T> {
public:
    TDivNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TDivNode *clone() const;
};

/**
  * TPowNode implements the power operation
  */
template<typename T>
class TPowNode : public TBinaryNodeOp<T> {
public:
    TPowNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TPowNode *clone() const;
};

/**
  * TSqrtNode<> implements the build-in sqrt() function.
  * for numbers of type T.
  */
template<typename T>
class TSqrtNode : public TUnaryNodeOp<T> {
public:
    TSqrtNode(TNode<T> *ANode);

    virtual void accept(TNodeVisitor<T>&);
    virtual TSqrtNode *clone() const;
};

/**
  * The class TSinNode<T> implements the build-in sinus function
  * for numbers of type T.
  */
template<typename T>
class TSinNode : public TUnaryNodeOp<T> {
public:
    TSinNode(TNode<T> *AParam);

    virtual void accept(TNodeVisitor<T>&);
    virtual TSinNode *clone() const;
};

/**
  * The class TCosNode<T> implements the build-in cosinus function
  * for numbers of type T.
  */
template<typename T>
class TCosNode : public TUnaryNodeOp<T> {
public:
    TCosNode(TNode<T> *AParam);

    virtual void accept(TNodeVisitor<T>&);
    virtual TCosNode *clone() const;
};

/**
  * The class TTanNode<T> implements the build-in tangens function
  * for numbers of type T.
  */
template<typename T>
class TTanNode : public TUnaryNodeOp<T> {
public:
    TTanNode(TNode<T> *AParam);

    virtual void accept(TNodeVisitor<T>&);
    virtual TTanNode *clone() const;
};

/**
  * The class TLnNode<T> implements the build-in logn function
  * for numbers of type T.
  */
template<typename T>
class TLnNode : public TUnaryNodeOp<T> {
public:
    TLnNode(TNode<T> *AParam);

    virtual void accept(TNodeVisitor<T>&);
    virtual TLnNode *clone() const;
};

/**
  * Any user defined function defined in a given library.
  * If this user defined function can't be displayed
  * in a graph if it has multiple parameters.
  * MULTIPLE PARAMETER ARE TO BE DONE!
  */
template<typename T>
class TFuncNode : public TUnaryNodeOp<T> {
private:
    std::string FName;

public:
    TFuncNode(const std::string& AName, TNode<T> *AParam);

    std::string name() const;

    virtual void accept(TNodeVisitor<T>&);
    virtual TFuncNode<T> *clone() const;
};

/**
  * TIfNode is a gread plus* for that library :).
  * example: sig(x) = IF(x > 0, 1, -1)
  * example: fib(n) = IF(n = 1, 1, n + fib(n - 1)), n element |N
  * example: t(x)   = IF(int(x) % 3 = 0, 0, IF(int(x) % 3 = 1, 1, 2))
  */
template<typename T>
class TIfNode : public TBinaryNodeOp<T> {
private:
    std::auto_ptr<TNode<T> > FCondition;

public:
    TIfNode(TNode<T> *ACondNode, TNode<T> *AThenNode, TNode<T> *AElseNode);

    TNode<T> *condition() const;
    TNode<T> *trueExpr() const;
    TNode<T> *falseExpr() const;

    virtual void accept(TNodeVisitor<T>&);
    virtual TIfNode *clone() const;
};

/**
  * The class TEquNode<> impelemnts the equation operation
  * on nodes for numbers of type T.
  * \todo: complete the Equation class interface and impelemntation.
  */
template<typename T>
class TEquNode : public TBinaryNodeOp<T> {
public:
    TEquNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TEquNode *clone() const;
};

template<typename T>
class TUnEquNode : public TBinaryNodeOp<T> {
public:
    TUnEquNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TUnEquNode *clone() const;
};

template<typename T>
class TGreaterNode : public TBinaryNodeOp<T> {
public:
    TGreaterNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TGreaterNode *clone() const;
};

template<typename T>
class TLessNode : public TBinaryNodeOp<T> {
public:
    TLessNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TLessNode *clone() const;
};

template<typename T>
class TGreaterEquNode : public TBinaryNodeOp<T> {
public:
    TGreaterEquNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TGreaterEquNode *clone() const;
};

template<typename T>
class TLessEquNode : public TBinaryNodeOp<T> {
public:
    TLessEquNode(TNode<T> *ALeft, TNode<T> *ARight);

    virtual void accept(TNodeVisitor<T>&);
    virtual TLessEquNode *clone() const;
};

} // namespace math

#include <math++/nodes.tcc>

#endif
