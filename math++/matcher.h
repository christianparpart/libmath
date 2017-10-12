///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: matcher.h,v 1.3 2002/05/07 12:38:55 cparpart Exp $
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
#ifndef libmath_matcher_h
#define libmath_matcher_h

#include <map>
#include <list>
#include <memory>
#include <string>

namespace math {

template<class T>
class TMatchRegistry {
public:
    TMatchRegistry();
    TMatchRegistry(const TMatchRegistry<T>&);

    /// defines given expression node as given id.
    void define(const std::string& AId, const TNode<T> *ANode);
    /// checks whether given id is defined or not.
    bool defined(const std::string& AId) const;
    /// returns expression node to given id.
    const TNode<T> *get(const std::string& AId) const;

    /// marks given node as used
    void mark(const TNode<T> *ANode);

    /// returns true when ANode is either marked as used or is defined as any
    bool contains(const TNode<T> *ANode) const;

private:
    typedef std::map<std::string, const TNode<T> *> TAnyMap;
    typedef std::list<const TNode<T> *> TNodeList;

    TAnyMap FAnyMap;
    TNodeList FNodeList;
};

////////////////////////////////////////////////////////////////////////
// The match template tree

template <class T>
class TMatch {
public:
    virtual ~TMatch() {}
    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const = 0;
};

template <class T>
class TNumMatch : public TMatch<T> {
public:
    TNumMatch(const T& ANum);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;

private:
    T FNumber;
};

template <class T>
class TAnyMatch : public TMatch<T> {
public:
    TAnyMatch(const std::string& AId);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;

private:
    std::string FIdent;
};

/** T2Match is the base class for operators where the operands may be
    equivalent exchaned (+, *) */
template <class T>
class T2Match : public TMatch<T> {
protected:
    T2Match(TMatch<T> *ALeft, TMatch<T> *ARight);
    ~T2Match();

    // gets additional match methods for share soon

    typedef std::list<TMatch<T> *> TList;

    TList FPatterns;
};

template <class T>
class TPlusMatch : public T2Match<T> {
public:
    TPlusMatch(TMatch<T> *ALeft, TMatch<T> *ARight, ...);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;
};

template <class T>
class TMulMatch : public T2Match<T> {
public:
    TMulMatch(TMatch<T> *ALeft, TMatch<T> *ARight, ...);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;
};

template <class T>
class TNegMatch : public TMatch<T> {
public:
    TNegMatch(TMatch<T> *ANode);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;

private:
    std::auto_ptr<TMatch<T> > FNode;
};

template <class T>
class TDivMatch : public TMatch<T> {
public:
    TDivMatch(TMatch<T> *ALeft, TMatch<T> *ARight);
    
    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;

private:
    std::auto_ptr<TMatch<T> > FLeft;
    std::auto_ptr<TMatch<T> > FRight;
};

template <class T>
class TPowMatch : public TMatch<T> {
public:
    TPowMatch(TMatch<T> *ABase, TMatch<T> *AExp);

    virtual bool match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const;

private:
    std::auto_ptr<TMatch<T> > FBase;
    std::auto_ptr<TMatch<T> > FExp;
};

/** TMatcher<> is a dynamic matching system for symbolic expressions.
  * One application for that is simplifying expressions.
  *
  * TMatcher<> class is really <b>to be done</b>. <i>But it will rock then</i>.
  * Example:
  * <pre>
  *   TMatcher<T>::TResult matchResult;
  *   if (TMatcher<T>::match("a+a", expr, machResult))
  *       return transform(expr, machResult, "2*a+$");
  * </pre>
  *
  * the "$" means the remaining part not matched using given template match,
  * here "a+a". Example: if you've a+b+a and want match a+a, then the remaining
  * part is b; if you've a^2+b^(sin(2x)+2)+c^2+c*2*a and want to match 
  * a^2+2ab+b^2, then the matched parts is: "a^2+c^2+c*2*a" and the remaining
  * part will be: "b^(sin(2x)+2)".
  */
template<class T>
class TMatcher : public TNodeVisitor<T> {
public:
    typedef std::map<std::string, TNode<T> > TResult;

    /** matchExact returns true when the template (AMatch) represents exactly
      * the test expression (AExpr).
      */
    static bool matchExact(const TMatch<T> *AMatch, const TNode<T> *AExpr, 
        TMatchRegistry<T> *AReg = 0);

    /** returns true when given pattern (AMatch) was matched in given 
      * expression (AExpr). The result data is stored into the registry
      * on success only. 
      */
    static bool match(const TMatch<T> *AMatch, const TNode<T> *AExpr,
        TMatchRegistry<T> *AReg = 0);

    /** match matches a given expression template on expression AExpr and
      * puts its result into AResult.
      */
    static unsigned match(const std::string& AMatch, const TNode<T> *AExpr, 
        TResult& AResult);

private:
    TMatcher(const TMatch<T> *AMatch, const TNode<T> *ANode,
        TMatchRegistry<T> *AReg = 0);

private:
    const TMatch<T> *FMatch;
    const TNode<T> *FExpr;

private:
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

#include <math++/matcher.tcc>

#endif
