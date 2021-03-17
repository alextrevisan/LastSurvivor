#ifndef _ARGS_H
#define _ARGS_H
#include <string>
#include <vector>
#include <algorithm>

class Args
{
    public:
        Args(int argc, char** argv)
        {
            args.reserve(argc);
            for(int i = 0; i < argc; ++i)
            {
                args.push_back(argv[i]);
            }
        }
        template<typename Type>
        Type Get(const std::string& param)
        {
            const auto &it = std::find(args.begin(), args.end(), param);

            if constexpr (std::is_same<Type, int>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, long>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, float>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, std::string>::value)
                return *(it+1);
            if constexpr (std::is_same<Type, const char*>::value)
                return *(it+1);
        }

        template<typename Type>
        Type Get(const std::string& param, const Type& defaultValue)
        {
            if(!Contains(param))
                return defaultValue;

            const auto &it = std::find(args.begin(), args.end(), param);

            if constexpr (std::is_same<Type, int>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, long>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, float>::value)
                return std::stoi(*(it+1));
            if constexpr (std::is_same<Type, std::string>::value)
                return *(it+1);
            if constexpr (std::is_same<Type, const char*>::value)
                return *(it+1);

            return defaultValue;
        }

        bool Contains(const std::string& param)
        {
            const auto it = std::find(args.begin(), args.end(), param);
             return it != args.end()
                && (it+1) != args.end();
        }
    protected:

    private:
        std::vector<std::string> args;
};

#endif // _ARGS_H
