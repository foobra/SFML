//
// Created by jtang on 2017/08/20.
//

#ifndef SFML_SMARTPOINTERDEFINES_HPP
#define SFML_SMARTPOINTERDEFINES_HPP


#include <memory>

template<typename T> using SP = std::shared_ptr<T>;
template<typename T> using UP = std::unique_ptr<T>;
template<typename T> using WP = std::weak_ptr<T>;


template<typename T, typename... Args>
std::shared_ptr<T> MakeSP(Args &&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
std::unique_ptr<T> MakeUP(Args &&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

static_assert(std::is_same<std::shared_ptr<int>, SP<int>>::value);
static_assert(std::is_same<std::unique_ptr<int>, UP<int>>::value);
static_assert(std::is_same<std::weak_ptr<int>, WP<int>>::value);

#endif //SFML_SMARTPOINTERDEFINES_HPP
