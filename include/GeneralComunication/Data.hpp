
#ifndef AIS2203_JETRACER_DATA_HPP
#define AIS2203_JETRACER_DATA_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

enum DrivingMode {
	UNKNOWN,

	FROM_GUI,
	MAP_ROOM,
	FOLLOW_HUMAN,
};
NLOHMANN_JSON_SERIALIZE_ENUM(DrivingMode, {
	{DrivingMode::UNKNOWN, "unknown"},
	{DrivingMode::FROM_GUI, "from_gui"},
	{DrivingMode::MAP_ROOM, "map_room"},
	{DrivingMode::FOLLOW_HUMAN, "follow_human"}
})

struct Position {
	double x;
	double y;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Position, x, y);
};

struct Person {
	Position pos;
	bool isSitting;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, pos, isSitting);
};

struct Steering {
	double steering;
	double throttle;

	double steering_bias;
	double steering_gain;
	double throttle_gain;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Steering, steering, throttle, steering_bias, steering_gain, throttle_gain);
};

struct DataStruct {
	DrivingMode driving_mode = FROM_GUI;
	Steering steering{};
	Position car_pos{};
	std::vector<Person> persons;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(DataStruct, driving_mode, steering, car_pos, persons);
};

class Data {
public:
	void fromJson(std::string &val);
	std::string toJson() const;

private:
	DataStruct _data;

};


#endif //AIS2203_JETRACER_DATA_HPP