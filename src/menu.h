#ifndef MENU_H
#define MENU_H
#include <functional>
#include <iostream>
#include <optional>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace beroset {

/*!
 * \brief Map which is constexpr-compatible
 *
 * \tparam Key may be an int or a std::string_value or the like.
 * \tparam Value must have an operator()() and ostream<< defined
 * \tparam Size is the number of options in the list
 */
template <typename Key, typename Value, std::size_t Size>
struct Map {
    std::array<std::pair<Key, Value>, Size> data;
    constexpr std::optional<Value> at(const Key& key) const {
        const auto itr = std::find_if(begin(data), end(data),
            [&key](const auto &v) { return v.first == key; });
        if (itr != end(data)) {
            return itr->second;
        } else {
            return {};
        }
    }
};

/*!
 * \brief ConsoleMenu is a constexpr-compatible text-based menu
 *
 * \tparam String may be std::string, std::string_view or similar
 * \tparam MenuEntry is a class representing each entry.  It's required to have ostream<< and operator()() defined.
 * \tparam ItemCount is the number of menu items
 */
template <class String, class MenuEntry, std::size_t ItemCount>
class ConsoleMenu {
    using IStream = std::basic_istream<typename String::value_type>;
    using OStream = std::basic_ostream<typename String::value_type>;
    using MapType = std::array<std::pair<String, MenuEntry>, ItemCount>;
public:
    constexpr ConsoleMenu(const String& message,
        const String& invalidChoiceMessage,
        const String& prompt,
        const String& delimiter,
        IStream &in,
        OStream &out,
        const MapType& commands
    );
    void operator()() const;
    OStream& showPrompt() const;
private:
    String message;
    String invalidChoiceMessage;
    String prompt;
    String delimiter;
    Map<String, MenuEntry, ItemCount> commands;
    IStream &in;
    OStream &out;
};

template <class String, class MenuEntry, std::size_t ItemCount>
constexpr ConsoleMenu<String, MenuEntry, ItemCount>::ConsoleMenu(const String& message,
    const String& invalidChoiceMessage,
    const String& prompt,
    const String& delimiter,
    IStream& in,
    OStream& out,
    const MapType& commands) :
        message{message},
        invalidChoiceMessage{invalidChoiceMessage},
        prompt{prompt},
        delimiter{delimiter},
        commands{commands},
        in{in},
        out{out}
{}

template <class String, class MenuEntry, std::size_t ItemCount>
std::basic_ostream<typename String::value_type>& ConsoleMenu<String, MenuEntry, ItemCount>::showPrompt() const {
    out << message;
    for (auto it{begin(commands.data)}; it != end(commands.data); ++it ) {
      out << it->first
            << delimiter
            << it->second
      << '\n';
    }
    return out;
}

template <class String, class MenuEntry, std::size_t ItemCount>
void ConsoleMenu<String, MenuEntry, ItemCount>::operator()() const {
    std::basic_string<typename String::value_type> userChoice;
    out << prompt;
    std::optional<MenuEntry> result;
    while ((!(in >> userChoice) || !(result = commands.at(userChoice)))) {
        std::cout << invalidChoiceMessage;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    result.value()();
}

} // namespace beroset

#endif // MENU_H
