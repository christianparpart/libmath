
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <math++/simplifier.h>
#include <math++/derive.h>
#include <math++/utils.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Expression Simplifier example program (i1)" << std::endl;

    try {
        std::string exprStr(argc == 2 ? argv[1] : "x^x");

        std::auto_ptr<math::TNode<double> >expr(math::TReader<double>::parse(exprStr));

        std::cout << "original   : "
                  << math::TPrinter<double>::print(expr.get())
                  << std::endl;

        for (int n = 1; n <= 4; ++n) {
            std::auto_ptr<math::TNode<double> >den(math::derive(expr.get(), n));

            std::cout << n << ". derivation : "
                      << math::TPrinter<double>::print(den.get())
                      << std::endl;
        }
    } catch (const math::EMath& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "further exception caught." << std::endl;
        return 1;
    }

    return 0;
}
