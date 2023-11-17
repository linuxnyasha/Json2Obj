#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <json2obj/json2obj.h>
struct Phone {
  unsigned long int number;
  std::string op;
  bool operator==(const Phone& other) const {
    return this->number == other.number && this->op == other.op;
  };
};
struct Human {
  std::string name;
  std::optional<std::vector<Phone>> phones;
  bool operator==(const Human& other) const {
    bool status;
    if(this->phones.has_value() == other.phones.has_value()) {
      if(this->phones.has_value() == true) {
        status = this->phones.value() == other.phones.value();
        goto re;
      };
      status = true;
    };
    re:
    return this->name == other.name && status;
  };
};
struct City {
  std::vector<Human> people;
  bool operator==(const City& other) const {
    return people == other.people;
  };
};
TEST_CASE("Complex object are serialized", "[serialize_reflection_complex]") {
  City city;
  city.people.push_back(Human{"User", {}});
  city.people[0].phones = std::vector<Phone>{{88005553535, "sha512sum"}};
  auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(city));
  nlohmann::json jsonManual = R"({"people":[{"name":"User","phones":[{"number":88005553535,"op":"sha512sum"}]}]})"_json;
  REQUIRE(json == jsonManual);
};
TEST_CASE("Complex object are serialized Without optional vector", "[serialize_reflex_complex]") {
  City city;
  city.people.push_back(Human{"User", {}});
  auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(city));
  nlohmann::json jsonManual = R"({"people":[{"name":"User","phones":null}]})"_json;
  REQUIRE(json == jsonManual);
};
TEST_CASE("Complex object are deserialized", "[deserialize_reflection_complex]") {
  City city;
  city.people.push_back(Human{"User", {}});
  city.people[0].phones = std::vector<Phone>{{88005553535, "sha512sum"}};
  nlohmann::json json = R"({"people":[{"name":"User","phones":[{"number":88005553535,"op":"sha512sum"}]}]})"_json;
  auto cityFromJson = Json2Obj::JsonSerializator::deserialize<City>(std::move(json));
  REQUIRE(city == cityFromJson);
};
TEST_CASE("Complex object are deserialized Without optional vector", "[deserialize_reflection_complex]") {
  City city;
  city.people.push_back(Human{"User", {}});
  nlohmann::json json = R"({"people":[{"name":"User","phones":null}]})"_json;
  auto cityFromJson = Json2Obj::JsonSerializator::deserialize<City>(std::move(json));
  REQUIRE(city == cityFromJson);
};
