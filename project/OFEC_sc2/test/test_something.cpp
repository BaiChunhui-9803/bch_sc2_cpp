#include "../utility/catch.hpp"

#include<iostream>

using namespace std;




class A {
public:
	A() {
		std::cout << "construction of A" << std::endl;
	}
	~A() {
		std::cout << "deconstruction of A" << std::endl;
	}
	A(A&& other) {
		std::cout << "move construction of A" << std::endl;
	}
};
A createA() {
	return A();
}


TEST_CASE("somethings", "[C++]")
{
	const A& a = createA();
	int stop = -1;
}