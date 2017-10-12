
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <string>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Expression Tree Iterator example program (i1)" << std::endl;

    try {
        std::string exprStr(argc == 2 ? argv[1] : "2 + 3 + 4 + 5 + 6");

        std::auto_ptr<math::TNode<double> >expr(math::TReader<double>::parse(exprStr));

        std::cout << "reproduction: ";
        math::TPrinter<double>::printOn(std::cout, expr.get());
        std::cout << std::endl;

        // now the iteration test
        for (math::TNode<double>::iterator i = expr->begin(); i != expr->end(); ++i) {
            std::cout << "current node: "
                      << math::TPrinter<double>::print(i.get())
                      << std::endl;
        }

    } catch (const math::EReadError& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;

        return 1;
    } catch (...) {
        return 1;
    }

    return 0;
}
