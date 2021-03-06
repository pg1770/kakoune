#ifndef option_types_hh_INCLUDED
#define option_types_hh_INCLUDED

#include "exception.hh"
#include "string.hh"
#include "units.hh"

#include <tuple>
#include <vector>
#include <unordered_set>

namespace Kakoune
{

inline String option_to_string(const String& opt) { return opt; }
inline void option_from_string(const String& str, String& opt) { opt = str; }

inline String option_to_string(int opt) { return to_string(opt); }
inline void option_from_string(const String& str, int& opt) { opt = str_to_int(str); }
inline bool option_add(int& opt, int val) { opt += val; return val != 0; }

inline String option_to_string(bool opt) { return opt ? "true" : "false"; }
inline void option_from_string(const String& str, bool& opt)
{
    if (str == "true" or str == "yes")
        opt = true;
    else if (str == "false" or str == "no")
        opt = false;
    else
        throw runtime_error("boolean values are either true, yes, false or no");
}

constexpr Codepoint list_separator = ':';

template<typename T>
String option_to_string(const std::vector<T>& opt)
{
    String res;
    for (size_t i = 0; i < opt.size(); ++i)
    {
        res += escape(option_to_string(opt[i]), list_separator, '\\');
        if (i != opt.size() - 1)
            res += list_separator;
    }
    return res;
}

template<typename T>
void option_from_string(const String& str, std::vector<T>& opt)
{
    opt.clear();
    std::vector<String> elems = split(str, list_separator, '\\');
    for (auto& elem: elems)
    {
        T opt_elem;
        option_from_string(elem, opt_elem);
        opt.push_back(opt_elem);
    }
}

template<typename T>
bool option_add(std::vector<T>& opt, const std::vector<T>& vec)
{
    std::copy(vec.begin(), vec.end(), back_inserter(opt));
    return not vec.empty();
}

template<typename T>
String option_to_string(const std::unordered_set<T>& opt)
{
    String res;
    for (auto it = begin(opt); it != end(opt); ++it)
    {
        if (it != begin(opt))
            res += list_separator;
        res += escape(option_to_string(*it), list_separator, '\\');
    }
    return res;
}

template<typename T>
void option_from_string(const String& str, std::unordered_set<T>& opt)
{
    opt.clear();
    std::vector<String> elems = split(str, list_separator, '\\');
    for (auto& elem: elems)
    {
        T opt_elem;
        option_from_string(elem, opt_elem);
        opt.insert(opt_elem);
    }
}

template<typename T>
bool option_add(std::unordered_set<T>& opt, const std::unordered_set<T>& set)
{
    std::copy(set.begin(), set.end(), std::inserter(opt, opt.begin()));
    return not set.empty();
}

constexpr Codepoint tuple_separator = '|';

template<size_t I, typename... Types>
struct TupleOptionDetail
{
    static String to_string(const std::tuple<Types...>& opt)
    {
        return TupleOptionDetail<I-1, Types...>::to_string(opt) +
               tuple_separator + escape(option_to_string(std::get<I>(opt)), tuple_separator, '\\');
    }

    static void from_string(memoryview<String> elems, std::tuple<Types...>& opt)
    {
        option_from_string(elems[I], std::get<I>(opt));
        TupleOptionDetail<I-1, Types...>::from_string(elems, opt);
    }
};

template<typename... Types>
struct TupleOptionDetail<0, Types...>
{
    static String to_string(const std::tuple<Types...>& opt)
    {
        return option_to_string(std::get<0>(opt));
    }

    static void from_string(memoryview<String> elems, std::tuple<Types...>& opt)
    {
        option_from_string(elems[0], std::get<0>(opt));
    }
};

template<typename... Types>
String option_to_string(const std::tuple<Types...>& opt)
{
    return TupleOptionDetail<sizeof...(Types)-1, Types...>::to_string(opt);
}

template<typename... Types>
void option_from_string(const String& str, std::tuple<Types...>& opt)
{
    auto elems = split(str, tuple_separator, '\\');
    if (elems.size() != sizeof...(Types))
        throw runtime_error("not enough elements in tuple");
    TupleOptionDetail<sizeof...(Types)-1, Types...>::from_string(elems, opt);
}

template<typename RealType, typename ValueType>
inline String option_to_string(const StronglyTypedNumber<RealType, ValueType>& opt)
{
    return to_string(opt);
}

template<typename RealType, typename ValueType>
inline void option_from_string(const String& str, StronglyTypedNumber<RealType, ValueType>& opt)
{
     opt = StronglyTypedNumber<RealType, ValueType>{str_to_int(str)};
}

template<typename RealType, typename ValueType>
inline bool option_add(StronglyTypedNumber<RealType, ValueType>& opt,
                       StronglyTypedNumber<RealType, ValueType> val)
{
    opt += val;
    return val != 0;
}

template<typename T>
bool option_add(T&, const T&)
{
    throw runtime_error("no add operation supported for this option type");
}

enum YesNoAsk
{
    Yes,
    No,
    Ask
};

inline String option_to_string(YesNoAsk opt)
{
    switch (opt)
    {
        case Yes: return "yes";
        case No:  return "no";
        case Ask: return "ask";
    }
    kak_assert(false);
    return "ask";
}

inline void option_from_string(const String& str, YesNoAsk& opt)
{
    if (str == "yes" or str == "true")
        opt = Yes;
    else if (str == "no" or str == "false")
        opt = No;
    else if (str == "ask")
        opt = Ask;
    else
        throw runtime_error("invalid value '" + str + "', expected yes, no or ask");
}

}

#endif // option_types_hh_INCLUDED
