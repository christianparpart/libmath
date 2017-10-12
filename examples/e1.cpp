
#include <math++/nodes.h>
#include <math++/printer.h>

#include <iostream>
#include <memory>

int main() {
    std::cout << "Expression Manipulation example program (e1)" << std::endl;

    // 2 + 3 * 4
    std::auto_ptr<math::TNode<double> > e(
        new math::TPlusNode<double>(
            new math::TNumberNode<double>(2),
            new math::TMulNode<double>(
                new math::TNumberNode<double>(3),
                new math::TNumberNode<double>(4)
            )
        )
    );

    std::cout << "reproduction : ";
    math::TPrinter<double>::printOn(std::cout, e.get());
    std::cout << std::endl;

    return 0;
}
