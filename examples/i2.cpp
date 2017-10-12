// Operand Iterator Example (/src/examples/i2.cpp)

#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <string>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Operand Iterator example program (i1)" << std::endl;

    try {
        std::string exprStr(argc == 2 ? argv[1] : "1*2 + (3 + 4) + 5*1 + 6 + 1*7");

        std::auto_ptr<math::TNode<double> >expr(math::TReader<double>::parse(exprStr));
        std::cout << "input string: " << exprStr << std::endl
                  << "reproduction: "
                  << math::TPrinter<double>::print(expr.get())
                  << std::endl;

        std::cout << "iterating expression tree by operands:" << std::endl;
        for (math::TNode<double>::operand_iterator i(expr.get()); i != i.end(); ++i) {
            std::cout << "current node: "
                      << math::TPrinter<double>::print(i.get())
                      << std::endl;
        }
    } catch (const math::EMath& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;

        return 1;
    } catch (...) {
        return 1;
    }

    return 0;
}
