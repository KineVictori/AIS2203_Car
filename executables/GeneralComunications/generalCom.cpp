
#include "GeneralComunication/GeneralComunication.hpp"
#include "GeneralComunication/Data.hpp"

int main() {

	GeneralComunication genCom{30123};

	Data data;
	genCom.setData(data);
	data = genCom.getData();
}