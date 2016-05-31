#include <mapbox/datatpl.hpp>

#include <iostream>

int main() {
    using namespace mapbox::datatpl;
    using namespace std::literals::string_literals;

    const property_map props{
        { "a", "test"s },
        { "b", 32ull },
        { "c", false },
    };

    const expression e{value{std::nullptr_t{}}};
    const expression e2{value{std::nullptr_t{}}};

    std::cout << to<std::string>(e, props) << std::endl;
    // std::cout << to<bool>(value{std::nullptr_t{}}, props) << std::endl;
    // std::cout << to<std::string>(value{std::nullptr_t{}}, props) << std::endl;
    // std::cout << to<std::string>(value{bool{true}}, props) << std::endl;
    // std::cout << to<std::string>(value{bool{false}}, props) << std::endl;
    // std::cout << to<std::string>(value{uint64_t{32}}, props) << std::endl;
    // std::cout << to<std::string>(value{int64_t{-16}}, props) << std::endl;
    // std::cout << to<std::string>(value{double{3.145}}, props) << std::endl;
    // std::cout << to<std::string>(value{"test"s}, props) << std::endl;
    // std::cout << to<std::string>(value{std::vector<value>{ std::string{"foo"}, double{4} }}, props) << std::endl;
    // std::cout << to<std::string>(value{std::unordered_map<std::string, value>{ { std::string{"foo"}, double{4} } }}, props) << std::endl;
    // std::cout << to<std::string>(has_property{"a"}, props) << std::endl;
    // std::cout << to<std::string>(has_property{"x"}, props) << std::endl;
    // std::cout << to<std::string>(concat{{"foo"s, "bar"s}}, props) << std::endl;
    // std::cout << to<std::string>(property_or{"a"s, "bar"s}, props) << std::endl;
    // std::cout << to<std::string>(property_is<value_is_string>{"a"s}, props) << std::endl;
    // std::cout << to<std::string>(ternary{"c"_p, "a"s, "b"s}, props) << std::endl;

    return 0;
}
