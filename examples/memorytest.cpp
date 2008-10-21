/* cpp-simple.cpp

   Simple libftdi-cpp usage

   This program is distributed under the GPL, version 2
*/

#include "ftdi.hpp"
#include <iostream>
using namespace Ftdi;

int main(int argc, char **argv)
{
   std::cerr << "Creating ct context" << std::endl;
   Context ct;
   {
      std::cerr << "Copying ct2 context" << std::endl;
      Context ct2 = ct;
   }

   std::cerr << "Copying ct3 context" << std::endl;
   Context ct3(ct);

   std::cerr << "Result: " << ct.vendor() << std::endl;

   return 0;
}
 
