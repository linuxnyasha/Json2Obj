#include <json2obj/json2obj.h>
#include <nlohmann/json.hpp>
#include <benchmark/benchmark.h>

template <std::size_t Size, typename Type = std::size_t>
constexpr auto createArray() {
  std::array<Type, Size> result;
  for(std::size_t i = 0; i < Size; i++) {
    result[i] = Type{i};
  };
  return result;
};

static void BMSerializeReflectionSizeT(benchmark::State& state) {
  constexpr std::size_t data = 8;
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(data));
  };
};
struct IndexStructure {
  std::size_t index;
};
static void BMSerializeReflectionSimpleType(benchmark::State& state) {
  constexpr auto data = IndexStructure{8};
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(data));
  };
};

static void BMSerializeReflectionSimpleTypeArray10(benchmark::State& state) {
  constexpr auto data = createArray<10, IndexStructure>();
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(data));
  };
};
static void BMSerializeReflectionSimpleTypeVector10(benchmark::State& state) {
  constexpr auto array = createArray<10, IndexStructure>();
  std::vector<IndexStructure> data(10);
  for(const auto& element : array) {
    data.push_back(element);
  };
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(data));
  };
};


struct House {
  std::size_t id;
};
struct City {
  std::vector<House> house;
};

static void BMSerializeReflectionComplexType(benchmark::State& state) {
  City city = {{{1}}};
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(city));
  };
};
static void BMSerializeReflectionComplexTypeArray10(benchmark::State& state) {
  City city = {{{1}}};
  auto cityArray = std::array<City, 10>{city, city, city, city, city, city, city, city, city, city};
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(cityArray));
  };
};
static void BMSerializeReflectionComplexTypeVector10(benchmark::State& state) {
  City city = {{{1}}};
  auto cityArray = std::vector<City>{city, city, city, city, city, city, city, city, city, city};
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(cityArray));
  };
};

static void BMSerializeReflectionComplexType10(benchmark::State& state) {
  City city = {{{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}, {10}}};
  for(auto _ : state) {
    auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(city));
  };
};



BENCHMARK(BMSerializeReflectionSizeT);
BENCHMARK(BMSerializeReflectionSimpleType);
BENCHMARK(BMSerializeReflectionSimpleTypeArray10);
BENCHMARK(BMSerializeReflectionSimpleTypeVector10);
BENCHMARK(BMSerializeReflectionComplexType);
BENCHMARK(BMSerializeReflectionComplexTypeArray10);
BENCHMARK(BMSerializeReflectionComplexTypeVector10);
BENCHMARK(BMSerializeReflectionComplexType10);


BENCHMARK_MAIN();

