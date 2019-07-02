/*!
 * @brief Sample code for ArgumentParser
 * @author  koturn
 * @file    main.cpp
 */
#include <iostream>

#include <ArgumentParser.hpp>


/*!
 * @brief Entry point of the program
 * @param [in] argc  A number of command-line arguments
 * @param [in] argv  Command line arguments
 * @return  Exit-status
 */
int
main(int argc, const char* argv[])
{
  try {
    ArgumentParser ap(argv[0]);
    ap.add('a', "apple", "Apple");
    ap.add('b', "banana", ArgumentParser::OptionType::kRequiredArgument, "Banana");
    ap.add('c', "cake", ArgumentParser::OptionType::kOptionalArgument, "Cake");
    ap.add("donuts", ArgumentParser::OptionType::kNoArgument, "Donuts");
    ap.add('e', ArgumentParser::OptionType::kRequiredArgument, "Egg", "ARG", "42");
    ap.add('h', "help", "Show help and exit this program");
    ap.parse(argc, argv);

    if (ap.get<bool>("help")) {
      ap.showUsage();
      return 0;
    }

    std::cout << "--- Arguments" << std::endl;
    std::vector<std::string> args = ap.getArguments();
    for (std::vector<std::string>::const_iterator itr = args.begin(); itr != args.end(); ++itr) {
      std::cout << *itr << std::endl;
    }

    std::cout << "--- Options" << std::endl;
    std::cout << "Apple: " << (ap.get<bool>("apple") ? "Yes" : "No") << std::endl;
    std::cout << "Banana: " << ap.get('b') << std::endl;
    std::cout << "Cake: " << ap.get("cake") << std::endl;
    std::cout << "Donuts: " << (ap.get<bool>("donuts") ? "Yes" : "No") << std::endl;
    std::cout << "Egg: " << ap.get<int>('e') << std::endl;

    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
