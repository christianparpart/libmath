
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <math++/simplifier.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Expression Simplifier example program (i1)" << std::endl;

    try {
        std::string exprStr(argc == 2 ? argv[1] : "2 + 3 + 4 + 5 + 6");

        std::auto_ptr<math::TNode<double> >expr(math::TReader<double>::parse(exprStr));

        std::cout << "original   : "
                  << math::TPrinter<double>::print(expr.get())
                  << std::endl;

        std::auto_ptr<math::TNode<double> >se(math::TSimplifier<double>::simplify(expr.get()));

        std::cout << "simplified : "
                  << math::TPrinter<double>::print(se.get())
                  << std::endl;

    } catch (const math::EMath& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;

        return 1;
    } catch (...) {
        std::cout << "further exception caught." << std::endl;
        return 1;
    }

    return 0;
}
