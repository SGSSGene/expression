# Expression

## Note:
This work is “Recreational C++” ( Alisdair Meredith  - https://www.youtube.com/watch?v=ovxNM865WaU )

## Inspiration:
This is inspired by André Bergner talk nhttps://www.youtube.com/watch?v=QYNRoeWJuAw.
The goal is to capture expressions and automatically propagate changes through different variables.

## Example
The example can be compiled with `g++ -std=c++17 example.cpp`

```
#include "expression.h"

#include <iostream>

using expression::operator ""_v;

int main()
{
    auto x = 5._v;
    auto x2 = 5._v;
    auto y = x2 * x;       // x2 * x
    auto z = x * y;        // x * x2 * x
    auto w = 2. * z / 3. ; // 2* x * x2 * x / 3.

    std::cout << "x:  " << x.getValue()  << "\n"; // print 5
    std::cout << "x2: " << x2.getValue() << "\n"; // print 5
    std::cout << "y:  " << y.getValue()  << "\n"; // print 25
    std::cout << "z:  " << z.getValue()  << "\n"; // print 125
    std::cout << "w:  " << w.getValue()  << "\n"; // print 83.3333

    x = 6;
    std::cout << "x:  " << x.getValue()  << "\n"; // print 6
    std::cout << "x2: " << x2.getValue() << "\n"; // print 5
    std::cout << "y:  " << y.getValue()  << "\n"; // print 30
    std::cout << "z:  " << z.getValue()  << "\n"; // print 180
    std::cout << "w:  " << w.getValue()  << "\n"; // print 120

    x2 = 7;
    std::cout << "x:  " << x.getValue()  << "\n"; // print 6
    std::cout << "x2: " << x2.getValue() << "\n"; // print 7
    std::cout << "y:  " << y.getValue()  << "\n"; // print 42
    std::cout << "z:  " << z.getValue()  << "\n"; // print 252
    std::cout << "w:  " << w.getValue()  << "\n"; // print 168

    return 0;
}
```
