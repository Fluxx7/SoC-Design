#pragma once
#include <string>
#include <format>
#include <iostream>
#include <utility>
#include <vector>
#include <type_traits>

// A wrapper class for easy ANSI formatting in std::format-based functions
template <typename T>
class f_ANSI {
    public: 
        T value;

        template <typename U>
        constexpr f_ANSI(U&& v) : value(std::forward<U>(v)) {}

        constexpr operator const T&() const { return value; }
        
};

// Creates an f_ANSI wrapped version of value
template <typename T>
constexpr f_ANSI<std::decay_t<T>> asf_ANSI(T&& value) {
    return f_ANSI<std::decay_t<T>>(std::forward<T>(value));
}


// Behaves like std::print but all arguments are treated as f_ASNI<> wrapped 

template<typename... Args>
void print_ANSI(std::format_string<f_ANSI<std::decay_t<Args>>...> fmt, Args&&... args) {
    std::print("{}", format_ANSI(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
void println_ANSI(std::format_string<f_ANSI<std::decay_t<Args>>...> fmt, Args&&... args) {
    std::println("{}", format_ANSI(fmt, std::forward<Args>(args)...));
}
// Behaves like std::format but all arguments are treated as f_ASNI<> wrapped 
template<typename... Args>
std::string format_ANSI(std::format_string<f_ANSI<std::decay_t<Args>>...> fmt, Args&&... args){
    return std::format(fmt, asf_ANSI(std::forward<Args>(args))...);
}








namespace std {
    template <class T>
    struct formatter<f_ANSI<T>, char> {
        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin();
            auto get_char = [&]() { return it != ctx.end() ? *it : 0; };

            char c = get_char();
            if (c == 0 || c == '}') {
                it = inner_formatter.parse(ctx);
                return it;
            }
            
            // Extract ANSI code (everything up to '>' or '}')
            while ((c = get_char()) != 0 && c != '}') {
                if (c == '>') {
                    ++it;
                    ctx.advance_to(it);
                    it = inner_formatter.parse(ctx);
                    c = get_char();
                    if (c != 0 && c != '}') {
                        throw format_error("Inner formatter failed to fully parse context");
                    }
                    break;
                }
                ansi_code += c;
                ++it;
            }

            if ((c = get_char()) != 0 && c!= '}') {
                throw format_error("Failed to fully parse context");
            }
            ctx.advance_to(it);
            return it;
        }

        auto format(const f_ANSI<T>& val, std::format_context& ctx) const {
            auto out = ctx.out();
            if (!ansi_code.empty())
                out = std::format_to(std::move(out), "\x1b[{}", ansi_code);
            // format for wrapped object here
            out = inner_formatter.format(val.value, ctx);
            if (!ansi_code.empty())
                out = std::format_to(std::move(out), "\x1b[0m");
            return out;
        }

        private:
            mutable std::string ansi_code;
            std::formatter<T> inner_formatter;
    };
}
