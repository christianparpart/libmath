
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Expression Reader example program (r1)" << std::endl;

    try {
        std::string exprStr(argc == 2 ? argv[1] : "2 + 3 * 4");

        std::auto_ptr<math::TNode<double> >expr(math::TReader<double>::parse(exprStr));

        std::cout << "reproduction: "
                  << math::TPrinter<double>::print(expr.get())
                  << std::endl;

    } catch (const math::EMath& e) {
        std::cout << "exception caught: " << e.reason() << std::endl;

        return 1;
    } catch (...) {
        return 1;
    }

    return 0;
}
