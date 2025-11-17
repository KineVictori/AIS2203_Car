
#include "GeneralComunication/Data.hpp"

void Data::fromJson(std::string &val) {

	nlohmann::json obj;
	try {
		obj = nlohmann::json::parse(val);
	} catch (nlohmann::json::parse_error &e) {
		return;
	}

	_data.driving_mode = obj.value("driving_move", DrivingMode::UNKNOWN);
	_data.steering = obj.value("steering", Steering{});
}

std::string Data::toJson() const {
	nlohmann::json obj;

	obj["driving_mode"] = _data.driving_mode;
	obj["steering"] = _data.steering;
	obj["car_pos"] = _data.car_pos;
	obj["persons"] = _data.persons;

	return obj.dump();
}
