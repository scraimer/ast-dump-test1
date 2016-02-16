#include "logger-test.h"

int main(int const argc, char const * const * const argv)
{
	Logger log;
	Logger* logger = &log;

	(*logger) << 1 << "Hello!" << logger->endl;

	return (0 + 1);
}
