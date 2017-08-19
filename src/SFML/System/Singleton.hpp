//
// Created by jtang on 17/2/26.
//

#ifndef SFML_SINGLETON_HPP
#define SFML_SINGLETON_HPP

namespace sf {
template<typename T>
class Singleton final {
public:
    static T &Get() {
        static T m_pInstance;
        return m_pInstance;
    }

    Singleton() = delete;

    ~Singleton() = delete;

    Singleton(Singleton const &) = delete;

    Singleton(Singleton &&) = delete;

    Singleton &operator=(Singleton const &) = delete;

    Singleton &operator=(Singleton &&) = delete;
};
}


#endif //SFML_SINGLETON_HPP
