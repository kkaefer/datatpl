#pragma once

#include <mapbox/geometry/feature.hpp>

namespace mapbox {
namespace datatpl {

using value = mapbox::geometry::value;
using property_array = std::vector<value>;
using property_map = std::unordered_map<std::string, value>;

struct property {
    const std::string name;
};

struct has_property {
    const std::string name;
    bool evaluate(const property_map& p) const {
        return p.find(name) != p.end();
    }
};

template <typename T> struct value_is {
    bool operator()(const value& v) {
        return v.is<T>();
    }
};

using value_is_null = value_is<std::nullptr_t>;
using value_is_string = value_is<std::string>;

struct value_is_number {
    bool operator()(const value& v) {
        return v.is<uint64_t>() || v.is<int64_t>() || v.is<double>();
    }
};

using value_is_boolean = value_is<bool>;
using value_is_array = value_is<mapbox::util::recursive_wrapper<property_array>>;
using value_is_map = value_is<mapbox::util::recursive_wrapper<property_map>>;

template <typename T> struct property_is {
    const std::string name;
    bool evaluate(const property_map& p) const {
        const auto it = p.find(name);
        return it != p.end() && T{}(it->second);
    }
};

using expression = mapbox::util::variant<value,
                                         property,
                                         has_property,
                                         property_is<value_is_null>,
                                         property_is<value_is_number>,
                                         property_is<value_is_string>,
                                         property_is<value_is_boolean>,
                                         property_is<value_is_array>,
                                         property_is<value_is_map>,
                                         mapbox::util::recursive_wrapper<struct property_or>,
                                         mapbox::util::recursive_wrapper<struct concat>,
                                         mapbox::util::recursive_wrapper<struct length>,
                                         mapbox::util::recursive_wrapper<struct add>,
                                         mapbox::util::recursive_wrapper<struct subtract>,
                                         mapbox::util::recursive_wrapper<struct multiply>,
                                         mapbox::util::recursive_wrapper<struct divide>,
                                         mapbox::util::recursive_wrapper<struct ternary>>;

namespace detail {

template <typename T> struct value_visitor;

template <> struct value_visitor<std::string> {
    std::string operator()(std::nullptr_t) const {
        return "";
    }
    std::string operator()(bool v) const {
        return v ? "true" : "false";
    }
    std::string operator()(uint64_t v) const {
        return std::to_string(v);
    }
    std::string operator()(int64_t v) const {
        return std::to_string(v);
    }
    std::string operator()(double v) const {
        return std::to_string(v);
    }
    std::string operator()(const std::string& v) const {
        return v;
    }
    std::string operator()(const property_array& v) const {
        std::string r;
        for (auto& s : v)
            r += mapbox::util::apply_visitor(*this, s);
        return r;
    }
    std::string operator()(const property_map&) const {
        return "";
    }
};

template <> struct value_visitor<double> {
    double operator()(std::nullptr_t) const {
        return 0;
    }
    template <typename V>
    std::enable_if_t<std::is_arithmetic<V>::value, double> operator()(V v) const {
        return v;
    }
    double operator()(const std::string&) const {
        // TODO: parse
        return true;
    }
    double operator()(const property_array& v) const {
        return operator()(value_visitor<std::string>{}(v));
    }
    double operator()(const property_map& v) const {
        return v.size();
    }
};

template <> struct value_visitor<bool> {
    bool operator()(std::nullptr_t) const {
        return false;
    }
    template <typename V>
    std::enable_if_t<std::is_arithmetic<V>::value, bool> operator()(V v) const {
        return v;
    }
    bool operator()(const std::string& v) const {
        return !v.empty();
    }
    bool operator()(const property_array& v) const {
        return operator()(value_visitor<std::string>{}(v));
    }
    bool operator()(const property_map& v) const {
        return v.size();
    }
};

template <typename E>
using is_expression = std::is_same<std::remove_const_t<std::remove_reference_t<E>>, expression>;
template <typename E> constexpr bool is_expression_v = is_expression<E>::value;

template <typename T> struct expression_visitor {
    const property_map& map;

    T operator()(const value& v) const {
        return mapbox::util::apply_visitor(value_visitor<T>{}, v);
    }
    T operator()(const property& p) const {
        const auto it = map.find(p.name);
        return it != map.end() ? mapbox::util::apply_visitor(value_visitor<T>{}, it->second) : T{};
    }
    template <typename V>
    auto operator()(const V& v) const
        -> std::enable_if_t<is_expression_v<decltype(v.evaluate(map))>, T> {
        return mapbox::util::apply_visitor(*this, v.evaluate(map));
    }
    template <typename V>
    auto operator()(const V& v) const
        -> std::enable_if_t<!is_expression_v<decltype(v.evaluate(map))>, T> {
        return value_visitor<T>{}(v.evaluate(map));
    }
};

} // namespace detail

template <typename T> T to(const expression& e, const property_map& p) {
    return mapbox::util::apply_visitor(detail::expression_visitor<T>{ p }, e);
}

struct property_or {
    const std::string name;
    const expression fallback;
    expression evaluate(const property_map& p) const {
        auto it = p.find(name);
        return it != p.end() ? it->second : fallback;
    }
};

struct concat {
    const std::vector<expression> values;
    std::string evaluate(const property_map& p) const {
        std::string r;
        for (auto& e : values)
            r += to<std::string>(e, p);
        return r;
    }
};

struct length {
    const expression first;
    uint64_t evaluate(const property_map& p) const {
        return to<std::string>(first, p).length();
    }
};

struct add {
    const expression first, second;
    double evaluate(const property_map& p) const {
        return to<double>(first, p) + to<double>(second, p);
    }
};

struct subtract {
    const expression first, second;
    double evaluate(const property_map& p) const {
        return to<double>(first, p) - to<double>(second, p);
    }
};

struct multiply {
    const expression first, second;
    double evaluate(const property_map& p) const {
        return to<double>(first, p) * to<double>(second, p);
    }
};

struct divide {
    const expression first, second;
    double evaluate(const property_map& p) const {
        return to<double>(first, p) / to<double>(second, p);
    }
};

struct ternary {
    const expression condition, first, second;
    const expression& evaluate(const property_map& p) const {
        return to<bool>(condition, p) ? first : second;
    }
};

// -------------------------------------------------------------------------------------------------

inline property operator"" _p(const char* str, std::size_t len) {
    return property{ { str, len } };
}

} // namespace datatpl
} // namespace mapbox
