#include "utils.hpp"
#include <iostream>
#include <boost/optional.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
template <typename T>
struct adl_serializer<boost::optional<T>> {
    static void to_json(json& j, const boost::optional<T>& opt) {
        if (opt == boost::none) {
            j = nullptr;
        } else {
            j = *opt; // this will call adl_serializer<T>::to_json which will
                      // find the free function to_json in T's namespace!
        }
    }

    static void from_json(const json& j, boost::optional<T>& opt) {
        if (j.is_null()) {
            opt = boost::none;
        } else {
            opt = j.template get<T>(); // same as above, but with
                              // adl_serializer<T>::from_json
        }
    }
};
NLOHMANN_JSON_NAMESPACE_END