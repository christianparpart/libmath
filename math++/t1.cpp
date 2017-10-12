
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <math++/derive.h>
#include <math++/simplifier.h>
#include <math++/library.h>
#include <math++/utils.h>
#include <math++/matcher.h>
//#include <math++/biginteger.h>

#include <cmath>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    try {
#if 0
#if 0
        math::TBigInteger a(1);
        math::TBigInteger b(2);
        math::TBigInteger c("3");
        math::TBigInteger d("3.1415");
        math::TBigInteger r(a + b);

        std::cout << a << std::endl
                  << b << std::endl
                  << c << std::endl
                  << d << std::endl
                  << r << std::endl
                  << "--------------------" << std::endl
                  << std::endl;
#endif
        math::TLibrary<double> library;
        library.insert(math::TConstant<double>("e", 2.1718));//M_E));
        library.insert(math::TConstant<double>("pi", 3.1415));//M_PI));
        library.insert(math::TFunction<double>("sig", "IF(x < 0, -1, 1)"));
        library.insert(math::TFunction<double>("fib", "IF(x <= 1, 1, x + fib(x - 1))"));

        std::cout << library.function("sig") << std::endl
                  << library.function("fib") << std::endl;

        math::TFunction<double> f("f", "sig(x)");
        std::cout << f << std::endl
                  << "\tf(-pi)=" << f.call(-library.value("pi"), library) << std::endl
                  << "\tf(+pi)=" << f.call(+library.value("pi"), library) << std::endl;

        math::TFunction<double> g("g", "fib(x)");
        std::cout << g << std::endl;
        for (int n = 1; n <= 5; ++n)
            std::cout << "\tg(" << n << ")=" << g.call(n, library) << std::endl;

        math::TFunction<double> h("h", "pi^sin(x/e)");
        std::cout << h << std::endl
                  << "\th(3)=" << h.call(3, library) << std::endl;
#else
#if 1
        math::TMatchRegistry<double> FMatchReg;
        // "a^2 + 2ab + b^2 + $0" = "(a + b)^2 + $0"
        math::TMatch<double> *match = new math::TPlusMatch<double>(
            new math::TPowMatch<double>(
                new math::TAnyMatch<double>("a"),
                new math::TNumMatch<double>(2)
            ),
            new math::TMulMatch<double>(
                new math::TNumMatch<double>(2),
                new math::TAnyMatch<double>("a"),
                new math::TAnyMatch<double>("b"), 0 /* mark end of list */
            ),
            new math::TPowMatch<double>(
                new math::TAnyMatch<double>("b"),
                new math::TNumMatch<double>(2)
            ), 0 /* mark end of list */
        );
        delete match;
#endif
#endif
    } catch (const math::EMath& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;

        return 1;
    } catch (const char *AMsg) {
        std::cout << "exception caught: " << AMsg << std::endl;

        return 2;
    } catch (...) {
        std::cout << "further exception caught." << std::endl;
        return 3;
    }
    return 0;
}
