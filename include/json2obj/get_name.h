#include <boost/pfr.hpp>
namespace Json2Obj { namespace Impl {
  template <auto ptr>
  consteval auto getName() noexcept {
    constexpr std::string_view sig = __PRETTY_FUNCTION__;
    constexpr std::string_view parsed = [](std::string_view sig) -> std::string_view {
      auto fieldPos = sig.find_last_of(".");
      sig.remove_prefix(fieldPos + 1);
      return sig.substr(0, sig.size() - 2);
    }(sig);
    return parsed;
  };
  template <typename T>
  struct Wrapper {
    T value;
  };
  template <typename T>
  extern const T fakeObject;
  template <std::size_t I, typename T>
  consteval auto getName() {
    return getName<Wrapper{&boost::pfr::get<I>(fakeObject<T>)}>();
  };
}; };
