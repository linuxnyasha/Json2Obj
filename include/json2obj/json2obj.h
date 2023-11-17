#include <boost/pfr.hpp>

namespace Json2Obj {
  template <typename T, typename Arr, std::size_t... I>
  constexpr auto combineTupleAndArray(T&& tuple, Arr&& arr, std::index_sequence<I...>) {
    return std::make_tuple(
       std::make_pair(std::move(arr[I])
      ,std::get<I>(std::move(tuple)))
    ...);
  };

  template<typename Arr, typename... Args>
  constexpr auto combineTupleAndArray(std::tuple<Args...>&& tuple, Arr&& arr) {
    return combineTupleAndArray(
       std::move(tuple)
      ,std::forward<Arr>(arr)
      ,std::make_index_sequence<sizeof...(Args)>{}
    );
  };

  template <typename T>
  constexpr auto toTuple(T&& obj) {
    return combineTupleAndArray(
       boost::pfr::structure_to_tuple(std::forward<T>(obj))
      ,boost::pfr::names_as_array<T>()
    );
  };

  template <typename T, typename F, std::size_t... I>
  constexpr void forEach(T&& tuple, F&& f, std::index_sequence<I...>) {
    (f(std::move(std::get<I>(tuple))), ...);
  };

  template <typename F, typename... Args>
  constexpr void forEach(std::tuple<Args...> tuple, F&& f) {
    forEach(
       std::move(tuple)
      ,std::move(f)
      ,std::make_index_sequence<sizeof...(Args)>()
    );
  };

  template <typename F, typename Tuple, std::size_t... I>
  constexpr auto map(Tuple&& tuple, F&& f, std::index_sequence<I...>) {
    return std::make_tuple(f(std::get<I>(tuple))...);
  };

  template <typename F, typename... Args>
  constexpr auto map(std::tuple<Args...>&& tuple, F&& f) {
    return map(
       std::move(tuple)
      ,std::forward<F>(f)
      ,std::make_index_sequence<sizeof...(Args)>()
    );
  };

  template <typename F, typename Tuple, std::size_t... I>
  constexpr bool anyOf(Tuple tuple, F f, std::index_sequence<I...>) {
    return (... || f(std::get<I>(tuple)));
  };

  template <typename F, typename... Args>
  constexpr bool anyOf(std::tuple<Args...> tuple, F f) {
    return anyOf(
       std::move(tuple)
      ,std::move(f)
      ,std::make_index_sequence<sizeof...(Args)>()
    );
  };

  template <typename T, typename Tuple, std::size_t... I>
  constexpr auto constructTypeFromTuple(Tuple&& tuple, std::index_sequence<I...>) {
    return T{std::get<I>(tuple)...};
  };

  template <typename T, typename... Args>
  constexpr auto constructTypeFromTuple(std::tuple<Args...>&& tuple) {
    return constructTypeFromTuple<T>(
       std::move(tuple)
      ,std::make_index_sequence<sizeof...(Args)>()
    );
  };

  template <typename Array, std::size_t... I>
  static constexpr auto arrayToTuple(Array&& array, std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(array)...);
  };

  template <std::size_t Size, typename Array>
  static constexpr auto arrayToTuple(Array&& array) {
    return arrayToTuple(std::move(array),  std::make_index_sequence<Size>());
  };

  template <typename T>
  struct isArray {
    template <typename U>
    static auto test(U* u) -> decltype(
        []<typename Type, std::size_t Size>(const std::array<Type, Size>&){}(*u), std::true_type{});
    static std::false_type test(...);

    static constexpr bool value = decltype(test(std::declval<T*>()))::value || std::is_array_v<T>;
  };

  template <typename T>
  struct isIterable {
    template <typename U>
    static auto test(U* u) -> decltype(std::begin(*u), std::end(*u), std::true_type{});
    static std::false_type test(...);

    static constexpr bool value = decltype(test(std::declval<T*>()))::value;
  };

  template <typename T>
  constexpr bool isVectorV = (isIterable<T>::value && !isArray<T>::value);

  template <typename T>
  struct itVector {
    static constexpr auto value = isVectorV<T>;
  };

  template <typename T, typename = void>
  struct IsOptional : public std::false_type {};

  template <typename T>
  struct IsOptional<T, std::void_t<
    decltype(std::declval<T>().has_value())>
  > : public std::true_type {};


  namespace JsonSerializator {

    template <typename Json, typename T, typename = void>
    struct CanBeJson : public std::false_type {};

    template <typename Json, typename T>
    struct CanBeJson<Json, T, std::void_t<
      decltype(std::declval<Json&>()[""] = std::declval<T>())>
    > : public std::true_type {};




    namespace Impl {
      template <typename Json, typename T>
      struct Deserialize;
      template <typename T, typename Json>
      struct Serialize;

      template <typename T, typename Names, typename Json, std::size_t... I>
      constexpr auto constructTupleFromJson(Names&& names, Json&& json, std::index_sequence<I...>) {
        return std::make_tuple(
          Deserialize<Json, decltype(boost::pfr::get<I>(std::declval<T>()))>()(
            std::move(json[std::get<I>(names)])
          )...
        );
      };

      template <typename ElementType, typename Json>
      constexpr std::vector<ElementType> constructVectorFromJson(Json&& json) {
        auto vec = json.template get<std::vector<Json>>();
        std::vector<ElementType> result;
        for(Json& element : std::move(vec)) {
          result.push_back(Deserialize<Json, ElementType>()(std::move(element)));
        };
        return result;
      };

      template <typename Json, typename ElementType>
      constexpr Json constructJsonFromVector(std::vector<ElementType>&& vector) {
        std::vector<Json> resultVector;
        for(std::size_t i = 0; i < vector.size(); i++) {
          resultVector.push_back(Serialize<ElementType, Json>()(std::move(vector[i])));
        };
        return resultVector;
      };

      template <typename T, typename Names, typename Json>
      constexpr auto constructTupleFromJson(Json&& json, Names&& names) {
        return constructTupleFromJson<T>(
           std::move(names)
          ,std::forward<Json>(json)
          ,std::make_index_sequence<std::tuple_size_v<Names>>()
        );
      };

      template <typename Json, typename T>
      constexpr auto constructJsonFromObj(T&& obj) {
        return constructTypeFromTuple<Json>(map(toTuple(std::forward<T>(obj))
            ,[]<typename ObjType>(std::pair<std::string_view, ObjType>& obj){
          if constexpr(CanBeJson<Json, decltype(obj)>::value) {
            return obj;
          } else {
            using SecondType = decltype(obj.second);
            return std::pair{
               std::move(obj.first)
              ,Serialize<SecondType, Json>()(
                 std::move(obj.second)
              )
            };
          };
        }));
      };

      template <typename Json, typename Array>
      constexpr Json constructJsonFromArray(Array&& array) {
        using ElementType = std::remove_pointer_t<decltype(std::begin(std::declval<Array>()))>;
        return constructTypeFromTuple<Json>(map(arrayToTuple<std::tuple_size_v<Array>>(std::move(array))
            ,[]<typename Type>(Type obj){
          return Serialize<ElementType, Json>()(std::move(obj));
        }));
      };


      template <typename T, typename Json>
      struct Serialize {
        public:
          constexpr Json operator()(T&& obj) {
            if constexpr(CanBeJson<Json, T>::value) {
              return obj;
            };
            if constexpr(IsOptional<T>::value) {
              if(!obj.has_value()) {
                return {};
              };
              using Type = std::remove_reference_t<decltype(obj.value())>;
              return Serialize<Type, Json>()(std::move(obj.value()));
            } else {
              if constexpr(isArray<T>::value) {
                return constructJsonFromArray<Json>(std::forward<T>(obj));
              } else if constexpr(isVectorV<T>) {
                using ElementType = std::remove_reference_t<decltype(*obj.begin())>;
                return constructJsonFromVector<Json, ElementType>(std::forward<T>(obj));
              } else {
                return constructJsonFromObj<Json>(std::forward<T>(obj));
              };
            };
          };
      };

      template <typename Json, typename T>
      struct Deserialize {
        constexpr T operator()(Json&& json) {
          if constexpr(IsOptional<T>::value) {
            if(json.empty()) {
              return {};
            };
            using Type = std::remove_reference_t<decltype(std::declval<T>().value())>;
            return Deserialize<Json, Type>()(std::move(json));
          } else {
            if constexpr(CanBeJson<Json, T>::value) {
              return std::move(json.template get<T>());
            } else if constexpr(isArray<T>::value) {
              using ElementType = std::remove_pointer_t<decltype(std::declval<T>().begin())>;
              return constructTypeFromTuple<T>(
                  map(boost::pfr::structure_to_tuple(
                    json.template get<std::array<Json, std::tuple_size_v<T>>>()
                  ),
                    [](auto obj){
                return Deserialize<Json, ElementType>()(std::move(obj));
              }));
            } else if constexpr(isVectorV<T>) {
              using ElementType = std::remove_reference_t<decltype(*std::declval<T>().begin())>;
              return constructVectorFromJson<ElementType, Json>(std::forward<Json>(json));
            } else {
              return constructTypeFromTuple<T>(
                constructTupleFromJson<T>(
                  std::move(json)
                  ,boost::pfr::names_as_array<T>()
                )
              );
            };
          };
        };
      };
    };

    template <typename Json, typename T>
    constexpr Json serialize(T&& obj) {
      return Impl::Serialize<T, Json>()(std::forward<T>(obj));
    };

    template <typename T, typename Json>
    constexpr T deserialize(Json&& json) {
      return Impl::Deserialize<Json, T>()(std::forward<Json>(json));
    };

  };
};
