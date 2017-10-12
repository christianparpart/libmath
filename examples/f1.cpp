
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <math++/derive.h>
#include <math++/simplifier.h>
#include <math++/library.h>
#include <math++/utils.h>

#include <cmath>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Function Library example program (l1)" << std::endl;

    try {
        math::TLibrary<double> library;
        library.insert(math::TConstant<double>("e", 2.1718));// M_E));
        library.insert(math::TConstant<double>("pi", 3.1415));// M_PI));
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

        if (argc == 3) {
            math::TFunction<double> u("u", argv[1]);
            std::cout << u << std::endl
                      << "\tu(" << argv[2] << ")=" 
                      << u.call(math::calculate<double>(argv[2]), library) << std::endl;
        } else
            std::cout << "You may also try your own function if you enter" << std::endl
                      << "an EXPRESSION followed by an test argument as parameter to f1." << std::endl;
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
