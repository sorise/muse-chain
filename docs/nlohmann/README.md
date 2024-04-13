## [nlohmann::json](https://github.com/nlohmann/json)
**介绍**：C++ Json库，易于使用，可以和STL无缝对接。

---
### 1. 声明与构造
通过CMake添加库，然后就可以直接使用。
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

# 使用 FetchContent 引入 nlohmann/json 库
include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.2 # 选择你需要的特定版本
)

FetchContent_MakeAvailable(json)

# 将你的项目添加到这里
add_executable(MyExecutable main.cpp)

# 链接 nlohmann/json 库
target_link_libraries(MyExecutable PRIVATE nlohmann_json::nlohmann_json)
```
头文件引入
```cpp
#include "nlohmann/json.hpp"
using json = nlohmann::json;
```
基础例子
```cpp
nlohmann::json user = {
        {"name", "John Doe"},
        {"age", 42},
        {"address", {
                {"street", "123 Main St"},
                {"city", "Anytown"},
                {"state", "CA"},
                {"zip", "12345"}
        }}
};
int age = user["age"].get<int>();
auto address = user["address"];
std::vector<std::pair<std::string, std::string>> addressVector;

for (auto it = address.begin(); it != address.end(); ++it) {
    addressVector.emplace_back(it.key(), it.value());
}

// 输出向量中的内容
std::cout << "Address Vector:" << std::endl;
for (const auto& pair : addressVector) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}

fmt::print("{}, {}\n", user.at("name"), age);
```
#### 1.1 纯粹声明
直接声明一个对象。
```cpp
json j1;
json j2 = json::object();
json j3 = json::array();
 
std::cout << j1.type_name() << std::endl; // output: null
std::cout << j2.type_name() << std::endl; // output: object
std::cout << j3.type_name() << std::endl; // output: array
```
#### 1.2 构造
```cpp
nlohmann::json val = nlohmann::json::object();
val["routers"] = nlohmann::json::array();
val["ca"] = nlohmann::json::array();

val["routers"].push_back({{"ip","127.0.0.1"}, {"port",8080}});
val["routers"].push_back({{"ip","127.0.0.7"}, {"port",15000}});
val["ca"].push_back({{"ip","127.0.0.2"}, {"port",10000}});
val["ca"].push_back({{"ip","127.0.0.3"}, {"port",10008}});

std::cout << val.dump() << std::endl;
/*
 {"ca":[
    {"ip":"127.0.0.2","port":10000},
    {"ip":"127.0.0.3","port":10008}
   ],
  "routers":[
    {"ip":"127.0.0.1","port":8080},
    {"ip":"127.0.0.7","port":15000}
   ]}
 * */
//输出到文件中
std::ofstream o("pretty.json");
o << std::setw(4) << val << std::endl;
```


### [2.类型转换](#)

#### 2.1 类型检测

```cpp
json j;

j.is_null();
j.is_boolean();
j.is_number();
j.is_object();
j.is_array();
j.is_string();
```

#### 2.2 array
如何创建、遍历、基本操作。

```cpp
json j;
j.push_back("foo");
j.push_back(1);
j.push_back(true);
// also use emplace_back
j.emplace_back(1.78);

// iterate the array
for (json::iterator it = j.begin(); it != j.end(); ++it) {
    std::cout << *it << '\n';
}

for (auto& element : j) {
    std::cout << element << '\n';
}

// other stuff
j.size();     // 4 entries
j.empty();    // false
j.type();     // json::value_t::array
j.clear();    // the array is empty again
```

#### 任意类型转换
nlohmann 提供了更为方便的方式，可以直接将结构体转换为json对象，反之亦然。
只需要在 person 结构体所在的命名空间下，定义函数to_json()和from_json()就可以轻松的完成任意类型到 json 对象的转换，以及 json 转换为任意对象。
* 函数to_json()和from_json()和你定义的数据类型在同一个命名空间中；
* 当你要在另外的文件中使用这两个函数时，要正确的包含它所在的头文件；
* 在from_json中要使用at()，因为当你读取不存在的名称时，它会抛出错误。
```cpp
// 首先定义一个结构体
struct person {
    std::string name;
    std::string address;
    int age;
};

void to_json(nlohmann::json& j, const person& p) {
    j = nlohmann::json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
}

void from_json(const nlohmann::json& j, person& p) {
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
}

person p {"Ned Flanders", "744 Evergreen Terrace", 60};
nlohmann::json j = p;
std::cout << j << std::endl;
// {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}

// conversion: json -> person
auto p2 = j.get<person>();

// that's it
assert(p.address == p2.address);
```

### API

#### dump
```cpp
string_t dump(const int indent = -1, const char indent_char = ' ', const bool ensure_ascii = false,
   const error_handler_t error_handler = error_handler_t::strict) 
```
* indent参数是缩进的空格数，如果设置为负值，则表示不进行缩进。
* indent_char参数是用于缩进的字符。
* ensure_ascii: 参数用于确保输出的ASCII编码。
* error_handler参数是一个可选的错误处理程序。
