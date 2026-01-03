#include "module1.h"
#include "module2.h"
#include "module3.h"
#include <iostream>
using namespace std; // cout находится в пространстве имен std и написав эту команду, можно избавиться
                     // от необходимости писать std::cout каждый раз

int main(int argc, char** argv)
{
    cout <<  "Hello world!" << "\n";

    cout << Module1::getMyName() << "\n";
    cout << Module2::getMyName() << "\n";
    cout << Module3::getMyName() << endl;

    using namespace Module1; 
    cout << getMyName() << "\n"; // (A) выводится John
    cout << Module2::getMyName() << "\n";
    cout << Module3::getMyName() << "\n";

    // using namespace Module2; // (B)
    // cout << getMyName() << "\n";      // COMPILATION ERROR (C) потому что существует 
    //                                   // более одного экземпляра интерпретации функции
    //                                   // нет одного очевидного значения 
    //                                   // getMyName() и в module1.h и в module2.h определена
    //                                   // и мы пытаемся вызвать две разные функции с одним названием, всё

    using Module2::getMyName;
    cout << getMyName() << "\n"; // (D) выводится James
}
