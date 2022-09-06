#include "../utility/catch.hpp"
#include "../utility/functional.h"
#include"../core/global.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/individual.h"
#include "../core/algorithm/algorithm.h"


#include<iostream>
using namespace std;
using namespace ofec;


TEST_CASE("utility", "[core_fun][calbest]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);
	
	auto id = ADD_RND(0.1);
	GET_RND(id).uniform.next();
	//GET_
	Population<Individual<>> p;

}