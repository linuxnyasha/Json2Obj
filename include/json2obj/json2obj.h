#include <boost/pfr.hpp>
#include <json2obj/get_name.h>
namespace Json2Obj {

#ifndef BOOST_PFR_CORE_NAME_ENABLED
#  define BOOST_PFR_CORE_NAME_ENABLED 0
#endif


  template <std::size_t I, typename T>
  consteval std::string_view getName() {
    if constexpr(BOOST_PFR_CORE_NAME_ENABLED) {
      return boost::pfr::get_name<I, T>();
    } else {
      return Impl::getName<I, T>();
    };
  };

  template <typename T>
  constexpr auto toTuple(T&& obj) {
    if constexpr(std::is_rvalue_reference_v<T>) {
      return boost::pfr::structure_to_tuple(std::move(obj));
    }
    return boost::pfr::structure_tie(obj);
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
  constexpr auto mapWithIndex(Tuple&& tuple, F&& f, std::index_sequence<I...>) {
    return std::make_tuple(f(boost::pfr::detail::size_t_<I>(), std::get<I>(tuple))...);
  };

  template <typename F, typename... Args>
  constexpr auto mapWithIndex(std::tuple<Args...>&& tuple, F&& f) {
    return mapWithIndex(
       std::move(tuple)
      ,std::forward<F>(f)
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
  template <typename Type, std::size_t Size>
  constexpr auto insert(
       std::array<Type, Size>&& array
      ,std::pair<std::size_t, Type&&>&& add) {
    array[add.first] = std::forward<Type>(add.second);
    return array;
  };
  template <typename F, typename Type, std::size_t Size, typename ResultType>
  constexpr std::array<ResultType, Size> map(
       const std::array<Type, Size>& array
      ,F&& f
      ,std::array<ResultType, Size>&& result
      ,std::size_t it) {
    if(it == Size) {
      return result;
    };
    return map(
       array
      ,std::forward<F>(f)
      ,insert<ResultType>(std::move(result)
      ,std::pair{it, f(array[it])})
      ,it + 1
    );
  };

  template <typename F, typename Type, std::size_t Size>
  constexpr auto map(const std::array<Type, Size>& array, F&& f) {
    using ResultType = decltype(f(array[0]));
    return map(
       array
      ,std::forward<F>(f)
      ,std::array<ResultType, Size>{}
      ,0
    );
  };

  template <typename F, typename Type, std::size_t Size, typename ResultType>
  constexpr std::array<ResultType, Size> map(
       std::array<Type, Size>&& array
      ,F&& f
      ,std::array<ResultType, Size>&& result
      ,std::size_t it) {
    if(it == Size) {
      return result;
    };
    return map(
       std::move(array)
      ,std::forward<F>(f)
      ,insert<ResultType>(std::move(result)
      ,std::pair{it, f(array[it])})
      ,it + 1
    );
  };

  template <typename F, typename Type, std::size_t Size>
  constexpr auto map(std::array<Type, Size>&& array, F&& f) {
    using ResultType = decltype(f(array[0]));
    return map(
       std::move(array)
      ,std::forward<F>(f)
      ,std::array<ResultType, Size>{}
      ,0
    );
  };


  template <typename T, typename... Type>
  constexpr auto pushBack(std::vector<T>&& source, Type&&... add) {
    (..., source.push_back(std::forward<Type>(add)));
    return source;
  };
  template <typename F, typename ResultVector, typename Source>
  constexpr auto map(F&& f, ResultVector&& result, Source&& source, std::size_t i) {
    if(i == std::size(source)) {
      return result;
    };
    return map(
       std::forward<F>(f)
      ,pushBack(std::forward<ResultVector>(result)
      ,f(source[i]))
      ,std::forward<Source>(source)
      ,i + 1
    );
  };
  template <typename F, typename T>
  constexpr auto map(const std::vector<T>& source, F&& f) {
    std::vector<decltype(f(source[0]))> result;
    result.reserve(source.size());
    return map(std::forward<F>(f), std::move(result), source, 0);
  };
  template <typename F, typename T>
  constexpr auto map(std::vector<T>&& source, F&& f) {
    std::vector<decltype(f(source[0]))> result;
    result.reserve(source.size());
    return map(std::forward<F>(f), std::move(result), std::move(source), 0);
  };


  template <typename F, typename Tuple, std::size_t... I>
  constexpr bool anyOf(Tuple&& tuple, F&& f, std::index_sequence<I...>) {
    return (... || f(std::get<I>(tuple)));
  };

  template <typename F, typename... Args>
  constexpr bool anyOf(std::tuple<Args...>&& tuple, F&& f) {
    return anyOf(
       std::move(tuple)
      ,std::forward<F>(f)
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

  template <typename Result, typename T, std::size_t Size, std::size_t... I>
  constexpr auto constructTypeFromArray(std::array<T, Size>&& array, std::index_sequence<I...>) {
    return Result{std::get<I>(array)...};
  };

  template <typename Result, typename T, std::size_t Size>
  constexpr auto constructTypeFromArray(std::array<T, Size>&& array) {
    return constructTypeFromTuple<Result>(
       std::move(array)
      ,std::make_index_sequence<Size>()
    );
  };


  template <typename Array, std::size_t... I>
  static constexpr auto arrayToTuple(Array&& array, std::index_sequence<I...>) {
    return std::tie(std::get<I>(array)...);
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

    static constexpr bool value = decltype(test(std::declval<std::remove_reference_t<T>*>()))::value || std::is_array_v<T>;
  };

  template <typename T>
  struct isVector {
    template <typename U>
    static auto test(U* u) -> decltype(
        []<typename Type>(const std::vector<Type>&){}(*u), std::true_type{});
    static std::false_type test(...);

    static constexpr bool value = decltype(test(std::declval<std::remove_reference_t<T>*>()))::value || std::is_array_v<T>;
  };

  template <typename T>
  struct isIterable {
    template <typename U>
    static auto test(U* u) -> decltype(std::begin(*u), std::end(*u), std::true_type{});
    static std::false_type test(...);

    static constexpr bool value = decltype(test(std::declval<T*>()))::value;
  };

  template <typename T>
  constexpr bool isVectorV = isVector<T>::value;


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
        return map(std::move(vec), [](auto obj){
          return Deserialize<Json, ElementType>()(std::move(obj));
        });
      };

      template <typename Json, typename ElementType>
      constexpr Json constructJsonFromVector(std::vector<ElementType>&& vector) {
        return map(std::move(vector), [](auto obj){
          return Serialize<ElementType&&, Json>()(std::move(obj));
        });
      };

      template <typename Json, typename ElementType>
      constexpr Json constructJsonFromVector(const std::vector<ElementType>& vector) {
        return map(vector, [](const ElementType& obj){
          return Serialize<const ElementType&, Json>()(obj);
        });
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
        return constructTypeFromTuple<Json>(mapWithIndex(toTuple(std::forward<T>(obj))
            ,[]<std::size_t I, typename ObjType>(boost::pfr::detail::size_t_<I>, ObjType& obj){
            return std::pair{
              getName<I, std::remove_cvref_t<T>>()
             ,Serialize<ObjType, Json>()(
                std::move(obj)
              )
          };
        }));
      };


      template <typename Json, typename Array>
      constexpr Json constructJsonFromArray(Array&& array) {
        using ElementType = std::remove_pointer_t<decltype(std::begin(std::declval<Array>()))>;
        return constructTypeFromArray<Json>(map(std::forward<Array>(array)
            ,[](ElementType obj){
          return Serialize<ElementType, Json>()(std::move(obj));
        }));
      };



      template <typename T, typename Json>
      struct Serialize {
        public:
          constexpr Json operator()(T&& obj) {
            if constexpr(IsOptional<T>::value) {
              if(!obj.has_value()) {
                return {};
              };
              using Type = std::remove_reference_t<decltype(obj.value())>;
              return Serialize<Type, Json>()(std::move(obj.value()));
            } else {
              if constexpr(CanBeJson<Json, T>::value) {
                return obj;
              } else if constexpr(isArray<T>::value) {
                return constructJsonFromArray<Json>(std::forward<T>(obj));
              } else if constexpr(isVectorV<T>) {
                return constructJsonFromVector<Json>(std::forward<T>(obj));
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
