#include "logger-test.h"

int main(int const argc, char const * const * const argv)
{
Logger log;
Logger* logger = &log;

(*logger) << 1 << "Good!" << logger->endl;
(*logger) << 1 << "Bad";

return (0 + 1);
}
