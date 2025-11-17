
#include "GeneralComunication/GeneralComunication.hpp"
#include "GeneralComunication/Data.hpp"

int main() {

	GeneralComunication genCom{30123};

	std::cout << "Press ENTER to stop server.\n";
	std::cin.get();
}