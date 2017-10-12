///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: matcher.tcc,v 1.4 2002/07/02 00:56:12 cparpart Exp $
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
#error You may not include math/matcher.tcc directly; include math/matcher.h instead.
#endif

#include <cstdarg>

namespace math {

///////////////////////////////////////////////////////////////////////
// TMatchRegistry
///////////////////////////////////////////////////////////////////////

template<class T>
TMatchRegistry<T>::TMatchRegistry() {
}

template<class T>
TMatchRegistry<T>::TMatchRegistry(const TMatchRegistry<T>& AProto) :
    FAnyMap(AProto.FAnyMap), FNodeList(AProto.FNodeList) {
}

template<class T>
void TMatchRegistry<T>::define(const std::string& AId, const TNode<T> *ANode) {
    FAnyMap[AId] = ANode;
}

template<class T>
bool TMatchRegistry<T>::defined(const std::string& AId) const {
    for (typename TAnyMap::const_iterator i = FAnyMap.begin(); i != FAnyMap.end(); ++i)
        if (i->first == AId)
            return true;

    return false;
}

template<class T>
const TNode<T> *TMatchRegistry<T>::get(const std::string& AId) const {
    for (typename TAnyMap::const_iterator i = FAnyMap.begin(); i != FAnyMap.end(); ++i)
        if (i->first == AId)
            return i->second;

    return 0; // should throw !
}

template<class T>
void TMatchRegistry<T>::mark(const TNode<T> *ANode) {
    FNodeList.push_back(ANode);
}

template<class T>
bool TMatchRegistry<T>::contains(const TNode<T> *ANode) const {
    for (typename TAnyMap::const_iterator i = FAnyMap.begin(); i != FAnyMap.end(); ++i)
        if (i->second == ANode)
            return true;

    for (typename TNodeList::const_iterator i = FNodeList.begin(); i != FNodeList.end(); ++i)
        if (*i == ANode)
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////
// Match Template Tree
///////////////////////////////////////////////////////////////////////

/* TNumMatch */
template<class T>
TNumMatch<T>::TNumMatch(const T& ANumber) : FNumber(ANumber) {
}

template<class T>
bool TNumMatch<T>::match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const {
    for (typename TNode<T>::const_operand_iterator i = AExpr; i != i.end(); ++i) {
        // find a number equal to the one we want to match and isn't yet matched
        // by another one.
        if (i->nodeType() == TNode<T>::NUMBER_NODE && !AReg->contains(i.get()) &&
                static_cast<const TNumberNode<T> *>(i.get())->number() == FNumber) {
            AReg->mark(i.get());
            return true;
        }
    }
    return false;
}

/* TAnyMatch */
template<class T>
TAnyMatch<T>::TAnyMatch(const std::string& AId) : FIdent(AId) {
}

template<class T>
bool TAnyMatch<T>::match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const {
    if (AReg->defined(FIdent)) {
        // identififier is already defined, look for another, but equal, one
        const TNode<T> *node = AReg->get(FIdent);
        for (typename TNode<T>::const_operand_iterator i = AExpr; i != i.end(); ++i) {
            if (!AReg->contains(i.get()) && *i == *node) {
                // if operand not yet in registry and equal to given one...
                AReg->mark(i.get()); // mark as already matched.
                return true;
            }
        }
    } else {
        // identifier not defined, search for first free operand
        for (typename TNode<T>::const_operand_iterator i = AExpr; i != i.end(); ++i) {
            if (!AReg->contains(i.get())) {
                // if operand not yet in registry, define and return success
                AReg->define(FIdent, i.get());
                return true;
            }
        }
    }
    return false;
}

/* T2Match */
template<class T>
T2Match<T>::T2Match(TMatch<T> *ALeft, TMatch<T> *ARight) {
    FPatterns.push_back(ALeft);
    FPatterns.push_back(ARight);
}

template<class T>
T2Match<T>::~T2Match() {
    for (typename TList::iterator i = FPatterns.begin(); i != FPatterns.end(); ++i) 
        delete *i;
}

/* TPlusMatch */
template<class T>
TPlusMatch<T>::TPlusMatch(TMatch<T> *ALeft, TMatch<T> *ARight, ...) :
    T2Match<T>(ALeft, ARight) {

    va_list ap;
    va_start(ap, ARight);

    while (TMatch<T> *p = va_arg(ap, TMatch<T> *))
        this->FPatterns.push_back(p);

    va_end(ap);
}

template<class T>
bool TPlusMatch<T>::match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const {
    // yet a primitive sequencial search
    for (typename T2Match<T>::TList::const_iterator p = this->FPatterns.begin(); p != this->FPatterns.end(); ++p)
        if (!TMatcher<T>::match(*p, AExpr, AReg))
            return false;

    return true;
}

/* TMulMatch */
template<class T>
TMulMatch<T>::TMulMatch(TMatch<T> *ALeft, TMatch<T> *ARight, ...) :
    T2Match<T>(ALeft, ARight) {

    va_list ap;
    va_start(ap, ARight);

    while (TMatch<T> *p = va_arg(ap, TMatch<T> *))
        this->FPatterns.push_back(p);

    va_end(ap);
}

template<class T>
bool TMulMatch<T>::match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const {
    return false;
}

/* TPowMatch */
template<class T>
TPowMatch<T>::TPowMatch(TMatch<T> *ABase, TMatch<T> *AExp) :
    FBase(ABase), FExp(AExp) {
}

template<class T>
bool TPowMatch<T>::match(const TNode<T> *AExpr, TMatchRegistry<T> *AReg) const {
    for (typename TNode<T>::const_operand_iterator i = AExpr; i != i.end(); ++i) {
        // look for a power node matching exactly our base and exponent template
        if (!AReg->contains(i.get()) && i->nodeType() == TNode<T>::POW_NODE) {
            if (TMatcher<T>::matchExact(FBase.get(), i->left()) &&
                    TMatcher<T>::matchExact(FExp.get(), i->right())) {
                AReg->mark(i.get());
                return true;
            }
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////
// TMatcher
///////////////////////////////////////////////////////////////////////

template<class T>
bool TMatcher<T>::matchExact(const TMatch<T> *AMatch, const TNode<T> *ANode,
    TMatchRegistry<T> *AReg) {
    TMatcher<T> matcher(AMatch, ANode, AReg);
    // TODO + FIXME
    return false;//!matcher.hasUnused();
}

template<class T>
bool TMatcher<T>::match(const TMatch<T> *AMatch, const TNode<T> *ANode,
    TMatchRegistry<T> *AReg) {
    TMatcher<T> matcher(AMatch, ANode, AReg);
    // TODO + FIXME
    return false;//matcher.success();
}

template<class T>
TMatcher<T>::TMatcher(const TMatch<T> *AMatch, const TNode<T> *ANode,
    TMatchRegistry<T> *AReg) : 
    FMatch(AMatch), FExpr(ANode) {
}

template<class T>
void TMatcher<T>::visit(TNumberNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TSymbolNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TParamNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TPlusNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TNegNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TMulNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TDivNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TPowNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TSqrtNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TSinNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TCosNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TTanNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TLnNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TFuncNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TIfNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TEquNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TUnEquNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TGreaterNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TLessNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TGreaterEquNode<T> *ANode) {
}

template<class T>
void TMatcher<T>::visit(TLessEquNode<T> *ANode) {
}

} // namespace math

