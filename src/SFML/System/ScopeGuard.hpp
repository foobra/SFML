//
// Created by jtang on 2017/08/28.
//

#ifndef SFML_SAFEGUARD_HPP
#define SFML_SAFEGUARD_HPP

#include <utility>

namespace sf {

    template<typename Fun>
    class ScopeGuard {
    public:
        explicit ScopeGuard(Fun &&f) : _fun(std::forward<Fun>(f)), _active(true) {
        }


        ~ScopeGuard() {
            if (_active) {
                _fun();
            }
        }


        void dismiss() {
            _active = false;
        }


        ScopeGuard() = delete;

        ScopeGuard(const ScopeGuard &) = delete;

        ScopeGuard &operator=(const ScopeGuard &) = delete;


        ScopeGuard(ScopeGuard &&rhs) noexcept : _fun(std::move(rhs._fun)), _active(rhs._active) {
            rhs.dismiss();
        }


    private:
        Fun  _fun;
        bool _active;
    };

    namespace detail {
        enum class ScopeGuardOnExit {
        };


        template<typename Fun>
        inline ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun &&fn) {
            return ScopeGuard<Fun>(std::forward<Fun>(fn));
        }
    }

#define __SCOPEGUARD_CONCATENATE_IMPL(s1, s2) s1##s2
#define __SCOPEGUARD_CONCATENATE(s1, s2) __SCOPEGUARD_CONCATENATE_IMPL(s1, s2)

#define ON_SCOPE_EXIT \
auto __SCOPEGUARD_CONCATENATE(ext_exitBlock_, __LINE__) = sf::detail::ScopeGuardOnExit() + [&]()
}

#endif //SFML_SAFEGUARD_HPP
