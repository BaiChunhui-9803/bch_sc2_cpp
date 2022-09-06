#include "prime_method.h"
#include "custom_method.h"
#include <iostream>
#include <ctime>

#ifdef OFEC_UNIT_TEST
#define CATCH_CONFIG_RUNNER
#include "../utility/catch.hpp"
#endif

int main(int argc, char* argv[]) {
	time_t timer_start, timer_end;
	ofec::registerProblem();
	ofec::registerAlgorithm();
	ofec::setAlgForPro();
	time(&timer_start);
#ifdef OFEC_UNIT_TEST
	int result = Catch::Session().run(argc, argv);
#else
	ofec::run();
#endif
	time(&timer_end);
	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;
	return 0;
}