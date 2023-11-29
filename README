# json2obj

## Description
**json2obj** is a C++20 project that provides static reflection for JSON serialization. It allows you to convert JSON objects to C++ objects and vice versa, making it easier to work with JSON data.

## Installation
```
git clone http://ivt5wiimhwpo56td6eodn2n3fduug3bglqvqewbk2jnyl4hcimea.b32.i2p/git/sha512sum/Json2Obj
cd Json2Obj
cmake . -GNinja
ninja
sudo ninja install
```

## Dependencies
To use the project, you need a compiler that supports the C++20 standard, boost pfr library and json library with nlohmann API.

## Usage
Your main task is to define C++ structures that correspond to the JSON object and use the `serialize` and `deserialize` functions for serialization and deserialization, respectively.

```cpp
#include <json2obj/json2obj.h>
#include <nlohmann/json.hpp>
#include <iostream>


struct Person {
  std::string name;
  int age;
};

int main() {
  // Data
  Person person{"Name", 42};

  // Serialization
  auto json = Json2Obj::JsonSerializator::serialize<nlohmann::json>(std::move(person));
  std::cout << json << std::endl; // {"age":42,"name":"Name"}

  // Deserialization
  Person personFromJson = Json2Obj::JsonSerializator::deserialize<Person>(std::move(json));
  std::cout << personFromJson.name << " " << personFromJson.age << std::endl; // Name 42
};

```
