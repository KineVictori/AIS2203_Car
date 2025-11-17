
#include <iostream>

#include "GeneralComunication/GeneralComunication.hpp"

int main() {

	GeneralComunication genCom{30123};

	std::cout << "Press ENTER to stop server.\n";
	std::cin.get();
}