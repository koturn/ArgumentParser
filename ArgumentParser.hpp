/*!
 * @file ArgumentParser.hpp
 * @brief Argument parser
 * @author koturn
 */
#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP  //!< Include guard

#include <cassert>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#if __cplusplus >= 201103L
#  include <type_traits>
#  include <unordered_map>
#else
#  include <map>
#endif  // __cplusplus >= 201103L
#if __cplusplus >= 201703L \
  || defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#  include <string_view>
#  define ARGUMENT_PARSER_HAS_STRING_VIEW 1
#endif  // __cplusplus >= 201703L


#if defined(__cplusplus) && __cplusplus >= 201103 \
  || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
  || defined(_MSC_VER) && (_MSC_VER > 1800 || (_MSC_VER == 1800 && _MSC_FULL_VER == 180021114))
//! Polyfill macro of @code noexcept @endcode
#  define ARGUMENT_PARSER_NOEXCEPT noexcept
#else
//! Polyfill macro of @code noexcept @endcode
#  define ARGUMENT_PARSER_NOEXCEPT throw()
#endif

#if defined(NDEBUG) && !defined(_DEBUG)
#  ifdef _MSC_VER
#    define ARGUMENT_PARSER_ASSUME(x)  __assume(x)
#  else
#    define ARGUMENT_PARSER_ASSUME(x)
#  endif  // _MSC_VER
#elif defined(assert)
#  define ARGUMENT_PARSER_ASSUME(x)  assert(x)
#else
#  define ARGUMENT_PARSER_ASSUME(x)
#endif  // defined(NDEBUG) && !defined(_DEBUG)

#if __cplusplus >= 201103L \
  || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
  || defined(_MSC_VER) && _MSC_VER >= 1600
#  define ARGUMENT_PARSER_EMPLACE_AVAILABLE
#endif


/*!
 * @class ArgumentParser
 * @brief Command-line argument parser
 */
class ArgumentParser
{
public:
#if __cplusplus >= 201103L \
  || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
  || defined(_MSC_VER) && _MSC_VER >= 1700
  /*!
   * @enum OptionType
   * @brief Option type which indicates whether option has an argument or not
   */
  enum class OptionType
  {
    //! Means taht an option requires no argument
    kNoArgument,
    //! Means taht an option requires an argument
    kRequiredArgument,
    /*!
     * @brief Means taht an option may or may not requires an argument
     *
     * For a short option, this enum value is equivalent to @code OptionType::kRequiredArgument @endcode .
     * But for a long option, an option isn't necessarily given argument.
     * If no argument is specified, "1" is set to an option value
     */
    kOptionalArgument
  };  // enum class OptionType
#else
  /*!
   * @brief Polyfill enum class @code OptionType @endcode for before-C++11
   */
  class OptionType
  {
  public:
    /*!
     * @enum OptionTypeEnum
     * @brief Actual enum values
     */
    enum OptionTypeEnum
    {
      //! Means taht an option requires no argument
      kNoArgument,
      //! Means taht an option requires an argument
      kRequiredArgument,
      /*!
       * @brief Means taht an option may or may not requires an argument
       *
       * For a short option, this enum value is equivalent to @code OptionType::kRequiredArgument @endcode .
       * But for a long option, an option isn't necessarily given argument.
       * If no argument is specified, "1" is set to an option value
       */
      kOptionalArgument
    };
    /*!
     * @brief Implicit convert from @code OptionType @endcode to @code OptionTypeEnum @endcode
     * @param [in] value  Enum value
     */
    OptionType(OptionTypeEnum value) ARGUMENT_PARSER_NOEXCEPT : value(value)
    {
    }
    /*!
     * @brief Cast overload to convert from @code OptionTypeEnum @endcode to @code OptionType @endcode
     * @return Actual enum value
     */
    operator OptionTypeEnum() const ARGUMENT_PARSER_NOEXCEPT
    {
      return value;
    }

  private:
    //! Actual enum value
    OptionTypeEnum value;
  };  // class OptionType
#endif

private:
  /*!
   * @struct OptionItem
   * @brief One option item
   */
  struct OptionItem
  {
    //! Short option name
    int shortOptName;
    //! Long option name
    std::string longOptName;
    //! Option type which indicates this option requires an argument or not
    OptionType optType;
    //! Description for this option
    std::string description;
    //! Meta variabl name
    std::string metavar;
    //! Value of this option
    std::string value;

    /*!
     * @brief Empty ctor
     */
    OptionItem()
      : shortOptName(-1)
      , longOptName()
      , optType(OptionType::kNoArgument)
      , description()
      , metavar()
      , value()
    {
    }

    /*!
     * @brief Ctor to make one option
     */
    OptionItem(
      int shortOptName,
      const std::string& longOptName,
      OptionType optType,
      const std::string& description,
      const std::string& metavar,
      const std::string& value)
      : shortOptName(shortOptName)
      , longOptName(longOptName)
      , optType(optType)
      , description(description)
      , metavar(metavar)
      , value(value)
    {
    }
  };


#if defined(ARGUMENT_PARSER_HAS_STRING_VIEW)
  /*!
   * @brief A meta function that identify whether T is @code std::string @endcode.
   * @tparam T  Target type parameter
   */
  template <typename T>
  struct is_stl_string
    : std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>
  {};  // struct is_stl_string

  /*!
   * @brief A meta function that identify whether T is @code std::string_view @endcode.
   * @tparam T  Target type parameter
   */
  template <typename T>
  struct is_stl_string_view
    : std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string_view>
  {};  // struct is_stl_string_view

  /*!
   * @brief A meta function that identify whether T is @code std::string @endcode or @code std::string_view @endcode.
   * @tparam T  Target type parameter
   */
  template <typename T>
  struct is_stl_string_type
    : std::disjunction<is_stl_string<T>, is_stl_string_view<T>>
  {};  // struct is_stl_string_type
#elif __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
  /*!
   * @brief A meta function that identify whether T is @code std::string_view @endcode.
   * @tparam T  Target type parameter
   */
  template <typename T>
  struct is_stl_string_type
    : std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::string>
  {};  // struct is_stl_string_type
#endif  // defined(ARGUMENT_PARSER_HAS_STRING_VIEW)


  //! Name of a program
  std::string progName;
  //! Indent string used in @code ArgumentParser::showUsage() @endcode
  std::string indentStr;
  //! Description for a program
  std::string description;
  //! Rest of command-line arguments not related to options
  std::vector<std::string> arguments;
  //! Options
  std::vector<OptionItem> options;
#if __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
  || defined(_MSC_VER) && _MSC_VER >= 1600
  //! Map which associates a short option name with an option attribute
  std::unordered_map<int, std::vector<OptionItem>::size_type> shortOptMap;
  //! Map which associates a long option name with an option attribute
  std::unordered_map<std::string, std::vector<OptionItem>::size_type> longOptMap;
#else
  //! Map which associates a short option name with an option attribute
  std::map<int, std::vector<OptionItem>::size_type> shortOptMap;
  //! Map which associates a long option name with an option attribute
  std::map<std::string, std::vector<OptionItem>::size_type> longOptMap;
#endif  // __cplusplus >= 201103L || defined(_MSC_VER) && _MSC_VER >= 1600

  /*!
   * @brief Get string which evaluate to @code true @endcode in @code std::stringstream @endcode or
   * @code std::stoi() @endcode.
   * @return std::string("1")
   */
  static std::string
  getTrueString() ARGUMENT_PARSER_NOEXCEPT
  {
    return std::string("1");
  }

  /*!
   * @brief Get string which evaluate to @code true @endcode in @code std::stringstream @endcode or @code std::stoi() @endcode
   * @return std::string("0")
   */
  static std::string
  getFalseString() ARGUMENT_PARSER_NOEXCEPT
  {
    return std::string("0");
  }

  /*!
   * @brief Convert std::string to desired type, T
   * @tparam T  Desired type
   * @param [in] x  String-value
   * @return Converted string
   */
  template <typename T>
  static T fromString(const std::string& x)
  {
    std::stringstream ss(x);
    T value;
    ss >> value;
    return value;
  }

#ifdef ARGUMENT_PARSER_HAS_STRING_VIEW
  /*!
   * @brief Convert C-string array to std::vector of std::string
   * @param [in] argc  Number of command-line arguments
   * @param [in] argv  Command-line arguments
   * @return std::vector of command-line arguments
   */
  static std::vector<std::string_view>
  cmdargsToStringViewVector(int argc, const char* argv[]) ARGUMENT_PARSER_NOEXCEPT
  {
    std::vector<std::string_view> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; i++) {
      args.emplace_back(argv[i]);
    }
    return args;
  }
#else
  /*!
   * @brief Convert C-string array to std::vector of std::string
   * @param [in] argc  Number of command-line arguments
   * @param [in] argv  Command-line arguments
   * @return std::vector of command-line arguments
   */
  static std::vector<std::string>
  cmdargsToStringVector(int argc, const char* argv[]) ARGUMENT_PARSER_NOEXCEPT
  {
    std::vector<std::string> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; i++) {
#  ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
      args.emplace_back(argv[i]);
#  else
      args.push_back(argv[i]);
#  endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
    }
    return args;
  }
#endif  // ARGUMENT_PARSER_HAS_STRING_VIEW

  /*!
   * @brief Split string at the first index of ch in the string
   * @tparam T  @code std::string @endcode or @code std::string_view @endcode
   * @param [in]  str     Target string
   * @param [in]  ch      Separator character
   * @param [out] first   First string of splitted string
   * @param [out] second  Second string of splitted string
   * @return Index of @code ch @endcode
   */
  template <typename T>
  static typename T::size_type
  splitFirstPos(const T& str, char ch, std::string& first, std::string& second) ARGUMENT_PARSER_NOEXCEPT
  {
#if __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    static_assert(
      is_stl_string_type<T>::value,
      "[ArgumentParser::splitFirstPos] T must be std::string or std::string_view (C++17 or later).");
#endif  // __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    typename T::size_type pos = str.find(ch);
    if (pos != std::string::npos) {
      first = str.substr(0, pos);
      second = str.substr(pos + 1);
    } else {
      first = str;
    }
    return pos;
  }

  /*!
   * @brief Parse one short option
   * @tparam T  @code std::string @endcode or @code std::string_view @endcode
   * @param [in] args  Argument vector
   * @param [in] idx   Current parsing index of command-line arguments
   * @return Index after parsing is complete
   */
  template <typename T>
  std::vector<std::string>::size_type
  parseShortOption(const std::vector<T>& args, std::vector<std::string>::size_type idx)
  {
#if __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    static_assert(
      is_stl_string_type<T>::value,
      "[ArgumentParser::parseShortOption] T must be std::string or std::string_view (C++17 or later).");
#endif  // __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    const T& optBody = args[idx];
    for (typename T::size_type i = 1; i < optBody.length(); i++) {
      int shortName = optBody[i];
      if (shortOptMap.find(shortName) == shortOptMap.end()) {
        throw std::runtime_error("Unknown option: -" + std::string(1, static_cast<char>(shortName)));
      }
      OptionItem& item = options[shortOptMap[shortName]];
      if (item.optType == OptionType::kNoArgument) {
        item.value = getTrueString();
      } else if (i == optBody.length() - 1) {
        if (idx + 1 >= args.size()) {
          throw std::runtime_error("Option requires an argument: -" + std::string(1, static_cast<char>(shortName)));
        }
        item.value = args[idx + 1];
        return idx + 1;
      } else {
        item.value = optBody.substr(i + 1);
        return idx;
      }
    }
    return idx;
  }

  /*!
   * @brief Parse one long option
   * @tparam T  @code std::string @endcode or @code std::string_view @endcode
   * @param [in] args  Argument vector
   * @param [in] idx   Current parsing index of command-line arguments
   * @return Index after parsing is complete
   */
  template <typename T>
  std::vector<std::string>::size_type
  parseLongOption(const std::vector<T>& args, std::vector<std::string>::size_type idx)
  {
#if __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    static_assert(
      is_stl_string_type<T>::value,
      "[ArgumentParser::parseShortOption] T must be std::string or std::string_view (C++17 or later).");
#endif  // __cplusplus >= 201103L || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
    std::string longOptName, value;
    typename T::size_type pos = splitFirstPos(args[idx].substr(2), '=', longOptName, value);
    std::vector<std::vector<OptionItem>::size_type> indices;
#if __cplusplus >= 201103L || defined(_MSC_VER) && _MSC_VER >= 1700
    for (const auto& kv : longOptMap) {
      if (kv.first.find(longOptName) == 0) {
        indices.push_back(kv.second);
      }
    }
#else
    for (std::map<std::string, std::vector<OptionItem>::size_type>::const_iterator itr = longOptMap.begin(); itr != longOptMap.end(); ++itr) {
      if (itr->first.find(longOptName) == 0) {
        indices.push_back(itr->second);
      }
    }
#endif  //  __cplusplus >= 201103L || defined(_MSC_VER) && _MSC_VER >= 1700

    if (indices.size() == 0) {
      throw std::runtime_error("Unknown option: --" + longOptName);
    } else if (indices.size() > 1) {
      throw std::runtime_error("Ambiguous option: --" + longOptName);
    }
    OptionItem& item = options[indices[0]];
    switch (item.optType) {
      case OptionType::kNoArgument:
        if (pos != std::string::npos) {
          throw std::runtime_error("Option doesn't take an argument: --" + longOptName);
        }
        item.value = getTrueString();
        return idx;
      case OptionType::kOptionalArgument:
        item.value = (pos == std::string::npos ? getTrueString() : value);
        return idx;
      case OptionType::kRequiredArgument:
        if (pos == std::string::npos) {
          if (idx + 1 >= args.size()) {
            throw std::runtime_error("Option requires an argument: --" + longOptName);
          }
          item.value = args[idx + 1];
          return idx + 1;
        } else {
          item.value = value;
          return idx;
        }
      default:
        ARGUMENT_PARSER_ASSUME(0);
        return static_cast<std::vector<std::string>::size_type>(-1);
    }
  }

  /*!
   * @brief Show description for one short option
   * @param [in] os    Output stream
   * @param [in] item  One option item
   */
  static void
  showShortOptionDescription(
    std::ostream& os,
    const OptionItem& item) ARGUMENT_PARSER_NOEXCEPT
  {
    os << "-" << static_cast<char>(item.shortOptName);
    if (item.optType != OptionType::kNoArgument) {
      os << " " << item.metavar;
    }
  }

  /*!
   * @brief Show description for one long option
   * @param [in] os    Output stream
   * @param [in] item  One option item
   */
  static void
  showLongOptionDescription(
    std::ostream& os,
    const OptionItem& item) ARGUMENT_PARSER_NOEXCEPT
  {
    os << "--" << item.longOptName;
    switch (item.optType) {
      case OptionType::kNoArgument:
        break;
      case OptionType::kOptionalArgument:
        os << "[=" << item.metavar << "]";
        break;
      case OptionType::kRequiredArgument:
        os << "=" << item.metavar;
        break;
      default:
        ARGUMENT_PARSER_ASSUME(0);
    }
  }

public:
  /*!
   * @brief A ctor which recieves program name only
   * @param [in] progName   Name of a program
   * @param [in] indentStr  Indent string
   */
  explicit ArgumentParser(
    const std::string& progName = "",
    const std::string& indentStr = "  ") ARGUMENT_PARSER_NOEXCEPT
    : progName(progName)
    , indentStr(indentStr)
    , description()
    , arguments()
    , options()
    , shortOptMap()
    , longOptMap()
  {
  }

  /*!
   * @brief Set program name
   * @param [in] progName_  A name of this program
   */
  void
  setProgramName(const std::string& progName_) ARGUMENT_PARSER_NOEXCEPT
  {
    progName = progName_;
  }

  /*!
   * @brief Set description of this program
   *
   * This descriptoon is used in showUsage().
   * @param [in] description_  A description of this program
   */
  void
  setDescription(const std::string& description_) ARGUMENT_PARSER_NOEXCEPT
  {
    description = description_;
  }

  /*!
   * @brief Set indent string
   *
   * This string is used in showUsage()
   *
   * @param [in] indentStr_  indent string
   */
  void
  setIndent(const std::string& indentStr_) ARGUMENT_PARSER_NOEXCEPT
  {
    indentStr = indentStr_;
  }

  /*!
   * @brief Get indent string for description
   * @return Return an indetn string with newline character
   */
  std::string
  getNewlineDescription() const ARGUMENT_PARSER_NOEXCEPT
  {
    return "\n" + indentStr + indentStr;
  }

  /*!
   * @brief Add an option to this parser
   * @param [in] shortOptName  Short option name
   * @param [in] longOptName   Long option name
   * @param [in] optType       Option type
   * @param [in] description_  Description for this option
   * @param [in] metavar       Name of meta variable
   * @param [in] defaultValue  Default value of this option
   */
  void
  add(
    int shortOptName,
    const std::string& longOptName,
    OptionType optType,
    const std::string& description_ = "",
    const std::string& metavar = "ARG",
    const std::string& defaultValue = "")
  {
    const std::string& dv = ((optType == OptionType::kNoArgument && defaultValue.empty()) ? getFalseString() : defaultValue);
    shortOptMap[shortOptName] = options.size();
    longOptMap[longOptName] = options.size();
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
    options.emplace_back(shortOptName, longOptName, optType, description_, metavar, dv);
#else
    options.push_back(OptionItem(shortOptName, longOptName, optType, description_, metavar, dv));
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
  }

  /*!
   * @brief Add a short name only option to this parser
   * @param [in] shortOptName  Short option name
   * @param [in] optType       Option type
   * @param [in] description_  Description for this option
   * @param [in] metavar       Name of meta variable
   * @param [in] defaultValue  Default value of this option
   */
  void
  add(
    int shortOptName,
    OptionType optType,
    const std::string& description_ = "",
    const std::string& metavar = "ARG",
    const std::string& defaultValue = "")
  {
    const std::string& dv = ((optType == OptionType::kNoArgument && defaultValue.empty()) ? getFalseString() : defaultValue);
    shortOptMap[shortOptName] = options.size();
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
    options.emplace_back(shortOptName, "", optType, description_, metavar, dv);
#else
    options.push_back(OptionItem(shortOptName, "", optType, description_, metavar, dv));
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
  }

  /*!
   * @brief Add a long name only option to this parser
   * @param [in] longOptName   Long option name
   * @param [in] optType       Option type
   * @param [in] description_  Description for this option
   * @param [in] metavar       Name of meta variable
   * @param [in] defaultValue  Default value of this option
   */
  void
  add(
    const std::string& longOptName,
    OptionType optType,
    const std::string& description_ = "",
    const std::string& metavar = "ARG",
    const std::string& defaultValue = "")
  {
    const std::string& dv = ((optType == OptionType::kNoArgument && defaultValue.empty()) ? getFalseString() : defaultValue);
    longOptMap[longOptName] = options.size();
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
    options.emplace_back(-1, longOptName, optType, description_, metavar, dv);
#else
    options.push_back(OptionItem(-1, longOptName, optType, description_, metavar, dv));
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
  }

  /*!
   * @brief Add a boolean option to this parser with a default value in any type
   * @param [in] shortOptName  Short option name
   * @param [in] longOptName   Long option name
   * @param [in] description_  Description for this option
   */
  void
  add(
    int shortOptName,
    const std::string& longOptName,
    const std::string& description_)
  {
    add(shortOptName, longOptName, OptionType::kNoArgument, description_, "", getFalseString());
  }

  /*!
   * @brief Add a short name only boolean option to this parser with a default value in any type
   * @param [in] shortOptName  Short option name
   * @param [in] description_  Description for this option
   */
  void
  add(
    int shortOptName,
    const std::string& description_)
  {
    add(shortOptName, OptionType::kNoArgument, description_, "", getFalseString());
  }

  /*!
   * @brief Add a long name only boolean option to this parser with a default value in any type
   * @param [in] longOptName   Long option name
   * @param [in] description_  Description for this option
   */
  void
  add(
    const std::string& longOptName,
    const std::string& description_)
  {
    add(longOptName, OptionType::kNoArgument, description_, "", getFalseString());
  }

  /*!
   * @brief Parse command-line arguments
   * @param [in] argc  The number of command-line arguments
   * @param [in] argv  Command-line arguments */
  void
  parse(
    int argc,
    const char* argv[])
  {
    progName = argv[0];
#ifdef ARGUMENT_PARSER_HAS_STRING_VIEW
    std::vector<std::string_view> args = cmdargsToStringViewVector(argc, argv);
    typedef std::vector<std::string_view>::size_type args_size_type;
#else
    std::vector<std::string> args = cmdargsToStringVector(argc, argv);
    typedef std::vector<std::string>::size_type args_size_type;
#endif  // ARGUMENT_PARSER_HAS_STRING_VIEW
    for (args_size_type i = 0; i < args.size(); i++) {
      if (args[i].find("--") == 0) {
        if (args[i].length() == 2) {
          for (i++; i < args.size(); i++) {
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
            arguments.emplace_back(args[i]);
#else
            arguments.push_back(args[i]);
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
          }
          return;
        }
        i = parseLongOption(args, i);
      } else if (args[i].find("-") == 0 && args[i].length() > 1) {
        i = parseShortOption(args, i);
      } else {
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
        arguments.emplace_back(args[i]);
#else
        arguments.push_back(args[i]);
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE
      }
    }
  }

  /*!
   * @brief Get not-option arguments in command-line arguments
   * @return Not-option arguments
   */
  std::vector<std::string>
  getArguments() const ARGUMENT_PARSER_NOEXCEPT
  {
    return arguments;
  }

  /*!
   * @brief Get an option value with a short option name
   * @param [in] shortOptName  Short options name
   * @return A string value related to specified short option name
   */
  std::string
  get(
    int shortOptName)
  {
    return options[shortOptMap[shortOptName]].value;
  }

  /*!
   * @brief Get an option value with a long option name
   * @param [in] longOptName  Long options name
   * @return A string value related to specified short option name
   */
  std::string
  get(
    const std::string& longOptName)
  {
    return options[longOptMap[longOptName]].value;
  }

  /*!
   * @brief Get an option value converted to desired type
   * @tparam T  Desired type
   * @param [in] shortOptName  Short options name
   * @return An option value converted to desired type
   */
  template <typename T>
  T
  get(int shortOptName)
  {
    return fromString<T>(get(shortOptName));
  }

  /*!
   * @brief Get an option value converted to desired type
   * @tparam T  Desired type
   * @param [in] longOptName  Long options name
   * @return An option value converted to desired type
   */
  template <typename T>
  T
  get(
    const std::string& longOptName)
  {
    return fromString<T>(get(longOptName));
  }

#if __cplusplus >= 201103L \
  || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
  || defined(_MSC_VER) && _MSC_VER >= 1600
  /*!
   * @brief Get option value converted to desired type
   * @tparam F  Functional object
   * @param [in] shortOptName  Short option name
   * @param [in] converter     std::string converter.
   * @return An option value converted to desired type
   */
  template <typename F>
  auto
  get(
    int shortOptName,
    const F& converter) -> decltype(converter(get(shortOptName)))
  {
    static_assert(
      std::is_constructible<std::function<decltype(converter(get(shortOptName)))(const std::string&)>, F>::value,
      "[ArgumentParser::get] Converter function signature must be T(std::string) or T([const] std::string&).");
    return converter(get(shortOptName));
  }

  /*!
   * @brief Get option value converted to desired type
   * @tparam F  Functional object
   * @param [in] longOptName  Long option name
   * @param [in] converter    std::string converter.
   * @return An option value converted to desired type
   */
  template <typename F>
  auto
  get(
    const std::string& longOptName,
    const F& converter) -> decltype(converter(get(longOptName)))
  {
    static_assert(
      std::is_constructible<std::function<decltype(converter(get(longOptName)))(const std::string&)>, F>::value,
      "[ArgumentParser::get] Converter function signature must be T(std::string) or T([const] std::string&).");
    return converter(get(longOptName));
  }
#else
  /*!
   * @brief Get option value converted to desired type
   * @tparam T  Desired type
   * @tparam F  Functional object
   * @param [in] shortOptName  Short option name
   * @param [in] converter     Converter (std::string -> T)
   * @return An option value converted to desired type
   */
  template <
    typename T,
    typename F
  >
  T
  get(
    int shortOptName,
    const F& converter)
  {
    return converter(get(shortOptName));
  }

  /*!
   * @brief Get option value converted to desired type
   * @tparam T  Desired type
   * @tparam F  Functional object
   * @param [in] longOptName  Long option name
   * @param [in] converter    Converter (std::string -> T)
   * @return An option value converted to desired type
   */
  template <
    typename T,
    typename F
  >
  T
  get(
    const std::string& longOptName,
    const F& converter)
  {
    return converter(get(longOptName));
  }
#endif

  /*!
   * @brief Get option value converted to desired type
   * @tparam T  Desired type (this type parameter is infered from convert function)
   * @param [in] shortOptName  Short option name
   * @param [in] convert       Converter (std::string -> T)
   * @return An option value converted to desired type
   */
  template <typename T>
  T
  get(
    int shortOptName,
    T (*convert)(const std::string&))
  {
    return convert(get(shortOptName));
  }

  /*!
   * @brief Get option value converted to desired type
   * @tparam T  Desired type (this type parameter is infered from convert function)
   * @param [in] longOptName  Long option name
   * @param [in] convert      Converter (std::string -> T)
   * @return An option value converted to desired type
   */
  template <typename T>
  T
  get(
    const std::string& longOptName,
    T (*convert)(const std::string&))
  {
    return convert(get(longOptName));
  }

  /*!
   * @brief Show usage of a program
   *
   * An output stream is std::cout (stdout).
   */
  void
  showUsage() const ARGUMENT_PARSER_NOEXCEPT
  {
    showUsage(std::cout);
  }

  /*!
   * @brief Show usage of a program
   * @param [in] os  Output stream
   */
  void
  showUsage(std::ostream& os) const ARGUMENT_PARSER_NOEXCEPT
  {
    if (!description.empty()) {
      os << description << "\n\n";
    }
    os << "[Usage]\n"
       << progName << " [Options ...] [Arguments ...]\n\n"
       << "[Options]\n";
#if __cplusplus >= 201103L \
    || defined(_MSVC_LANG) && _MSVC_LANG >= 201103L \
    || defined(_MSC_VER) && _MSC_VER >= 1700
    for (const auto& item : options) {
#else
    for (std::vector<OptionItem>::const_iterator itr = options.begin(); itr != options.end(); ++itr) {
      const OptionItem& item = *itr;
#endif  //  __cplusplus >= 201103L || defined(_MSC_VER) && _MSC_VER >= 1700
      os << indentStr;
      if (item.longOptName.empty()) {
        showShortOptionDescription(os, item);
      } else if (item.shortOptName == -1) {
        showLongOptionDescription(os, item);
      } else {
        showShortOptionDescription(os, item);
        os << ", ";
        showLongOptionDescription(os, item);
      }
      os << "\n" << indentStr << indentStr << item.description << std::endl;
    }
  }

  /*!
   * @brief Operator overload for output stream
   * @tparam CharT   Character type
   * @tparam Traits  Type traits
   * @param [in] os     Output stream
   * @param [in] this_  Refrence to this ArgumentParser object
   * @return Output stream
   */
  template <typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits>&
  operator<<(
    std::basic_ostream<CharT, Traits>& os,
    const ArgumentParser& this_)
  {
    this_.showUsage(os);
    return os;
  }

  /*!
   * @brief Operator overload for input stream
   * @tparam CharT   Character type
   * @tparam Traits  Type traits
   * @param [in] is     Input stream
   * @param [in] this_  Refrence to this ArgumentParser object
   * @return Input stream
   */
  template <typename CharT, typename Traits>
  friend std::basic_istream<CharT, Traits>&
  operator>>(
    std::basic_istream<CharT, Traits>& is,
    ArgumentParser&)
  {
    return is;
  }
};  // class ArgumentParser


#undef ARGUMENT_PARSER_NOEXCEPT
#undef ARGUMENT_PARSER_ASSUME
#ifdef ARGUMENT_PARSER_HAS_STRING_VIEW
#  undef ARGUMENT_PARSER_HAS_STRING_VIEW
#endif  // ARGUMENT_PARSER_HAS_STRING_VIEW
#ifdef ARGUMENT_PARSER_EMPLACE_AVAILABLE
#  undef ARGUMENT_PARSER_EMPLACE_AVAILABLE
#endif  // ARGUMENT_PARSER_EMPLACE_AVAILABLE


#endif  // ARGUMENT_PARSER_HPP
