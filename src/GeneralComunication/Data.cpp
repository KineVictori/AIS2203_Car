
#include <iostream>

#include "GeneralComunication/Data.hpp"

void Data::fromJson(std::string &val) {

	nlohmann::json obj;
	try {
		obj = nlohmann::json::parse(val);
	} catch (nlohmann::json::parse_error &e) {
		std::cerr << "Invalid json, error: " << e.what() << std::endl;
		return;
	}

	_recievingData.steering = obj.value("steering", Steering{});
	_recievingData.driving_mode = obj.value("driving_move", DrivingMode::UNKNOWN);

	_sensingData.steering = _recievingData.steering;
}

std::string Data::toJson() const {
	nlohmann::json obj;

	obj["car_pos"] = _sensingData.car_pos;
	obj["persons"] = _sensingData.persons;
	obj["steering"] = _sensingData.steering;

	return obj.dump();
}
