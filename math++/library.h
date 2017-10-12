///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: library.h,v 1.6 2002/04/29 19:34:30 cparpart Exp $
//  (module info here)
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
#ifndef libmath_library_h
#define libmath_library_h

#include <math++/error.h>

#include <list>
#include <map>
#include <string>

namespace math {

template<class> class TNode;
template<class> class TLibrary;

/**
  * TFunction<> is used for multiple function management as done by TLibrary<>.
  */
template<typename T>
class TFunction {
private:
    std::string FName;
    TNode<T> *FExpression;

public:
    TFunction();
    TFunction(const TFunction<T>&);
    TFunction(const std::string& AName, const std::string& AExprStr = std::string());
    TFunction(const std::string& AName, const TNode<T> *AExprTree);
    ~TFunction();

    T call(const T& AParam, const TLibrary<T>& ALibrary, unsigned ALimit = 64) const;

    void name(const std::string&);
    std::string name() const;

    void expression(const TNode<T> *ACopyOf);
    void expression(const std::string& AExprStr);
    TNode<T> *expression() const;
};

/**
  * TConstant<> is used to store constants, such as e or pi, in a TLibrary<>.
  */
template<typename T>
class TConstant {
private:
    std::string FName;
    T FValue;

public:
    TConstant();
    TConstant(const TConstant<T>&);
    TConstant(const std::string& AName, const T& AValue = T());

    void name(const std::string&);
    std::string name() const;

    void value(const T&);
    T value() const;
};

/**
  * ELibraryLookup is thrown by the TLibrary<> class when ever any lookup fails
  * or any elements in a library are found multiple.
  */
class ELibraryLookup : public EMath {
public:
    ELibraryLookup(const std::string& AMsg) : EMath(AMsg) {}
};


/**
  * TLibrary<> is used to manage multiple functions and constants
  * to be shared and to be called each other.
  * Note, that you can't have a function called f and a constant called f.
  */
template<typename T>
class TLibrary {
private:
    typedef std::list<TFunction<T> > TFunctionList;
    typedef std::list<TConstant<T> > TConstantList;
    
    TFunctionList FFunctions;
    TConstantList FConstants;

    void removeIf(const std::string& AName, bool AReplaceIfExists);

public:
    TLibrary();
    TLibrary(const TLibrary<T>&);

    /// inserts given function into library, it throws if it's duplicated
    void insert(const TFunction<T>&, bool AReplaceIfExists = false);
    /// inserts given constant into library, it throws if it's duplicated
    void insert(const TConstant<T>&, bool AReplaceIfExists = false);

    /// removes function or constant called AName
    void remove(const std::string& AName);

    /// returns reference to requested function, throws if not found
    TFunction<T> function(const std::string& AName) const;

    /// returns reference to requested constant, throws if not fuond
    TConstant<T> constant(const std::string& AName) const;

    /// returns true, if function (AName) exists
    bool hasFunction(const std::string& AName) const;

    /// returns true, if constant (AName) exists
    bool hasConstant(const std::string& AName) const;

    /// call() looks for a function AName calls it using AParam and returns its result. It throws on lookup error.
    T call(const std::string& AName, const T& AParam) const;
    /// value() look for a constant AName and returns its value. It throws on lookup error.
    T value(const std::string& AName) const;

    /// returns the number of functions stored in this library.
    unsigned functions() const;
    /// returns the number of constants stored in this library.
    unsigned constants() const;
};

} // namespace math

template<typename T> std::ostream& operator<<(std::ostream&, const math::TFunction<T>&);

#include <math++/library.tcc>

#endif
