#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

#include "mymath.h"

void calc(char *buff)
{
    std::string s(buff);
    std::stringstream ss(s);
    std::string word;
    std::vector<int> vec;
    while (ss >> word)
    {
        if (word.size() == 1 && isdigit(word[0]))
            vec.push_back(word[0] - '0');
    }
    if (vec.empty()) return;
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    int max = *std::max_element(vec.begin(), vec.end());
    int min = *std::min_element(vec.begin(), vec.end());
    std::sort(vec.rbegin(), vec.rend());

    std::cout << "Sum = " << sum << ", " <<
                 "max = " << max << ", " <<
                 "min = " << min << "\n";
    for (auto &i : vec) std::cout << i << ' ';
    std::cout << std::endl << std::endl;
}
