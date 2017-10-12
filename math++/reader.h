///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: reader.h,v 1.6 2002/04/20 06:35:05 cparpart Exp $
//  (This file contains the reader (parser) specific interface)
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
#ifndef libmath_reader_h
#define libmath_reader_h

#include <math++/error.h>

#include <string>

namespace math {

template<class> class TNode;

/**
  * EReadError is thrown while parsing the input expression string 
  * whenever an error occurs.
  */
class EReadError : public EMath {
public:
    EReadError(const std::string& AReason) : EMath(AReason) {}
};

/**
  * TReader<> represents the expression reader (aka. parser) wich generates
  * on given input (usually of type std::string) the equivalent output
  * of type TNode<>.
  */
template<class T>
class TReader {
private:
    /// TToken represents the id of an atom of an expression to be parsed.
    enum TToken {
        /* TAKE CARE : THE VALUES ARE HARD CODED */
        tkInvalid, tkEnd,
        tkNumber = 1000, tkSymbol,
        tkUnEqu, tkLess, tkGreater, tkLessEqu, tkGreaterEqu,
        tkVeryLess, tkVeryGreat,
        tkEqu = '=', tkComma = ',',
        tkPlus = '+', tkMinus = '-', tkMul = '*', tkDiv = '/', tkPow = '^',
        tkRndOpen = '(', tkRndClose = ')', tkBrOpen = '[', tkBrClose = ']',
        tkAngOpen = '<', tkAngClose = '>', tkSetOpen = '{', tkSetClose = '}'
    };

    std::string FExprStr;       // holds the current parsed expression
    std::string::size_type FPos;// holds the current read index

    TToken FToken;              // holds current parsed token id
    T FNumber;                  // holds last read number
    std::string FSymbol;        // holds last read symbol

private:
    TReader(const std::string& AInput);

    /// parses an expression on equation level (=,<>, <=, >=, <, >, <<, >>)
    TNode<T> *equation(bool get);
    // parses an expression (really any expression, even sub equations)
    TNode<T> *expr(bool get);
    /// parses whole expressions (+,-)...
    TNode<T> *simpleExpr(bool get);
    /// parses terms (*,/)...
    TNode<T> *term(bool get);
    /// parses facotrs (^)...
    TNode<T> *factor(bool get);
    /// parses that thing with the highest priority (e.g. numbers, idents, ...)
    TNode<T> *prim(bool get);

    /// creates a symbol node according to the current parsed symbol name
    TNode<T> *createSymbol();
    /// parses the parameter of a function (including the brackets)
    TNode<T> *param();

    /// returns true if parser reached end of input
    bool eof() const;

    /// reads next token, stores its values and returns its token-id
    TToken nextToken();
    /// reads any math operator stores it to FToken and returns true on success
    bool readOperator();
    /// reads a number and returns true on success, result is stored in FNumber
    bool readNumber();
    /// reads a symbol and returns true on success, result is stored in FSymbol
    bool readSymbol();

    /// returns the string according to the given token.
    static std::string tok2str(TToken AToken);
    /// expects current token as given and throws on fail, otherwise reads next token.
    void consume(TToken AToken);

public:
    /**
      * Parses given expression string (AInput) and returns its result.
      * Equation parsing is enabled if AEquation is !false.
      */
    static TNode<T> *parse(const std::string& AInput, bool AEquation = false);
};

} // namespace math

#include <math++/reader.tcc>

#endif
