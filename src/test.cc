// #include <iostream>
// #include <regex>
// #include <string>

// using namespace std;

// int main()
// {

//     std::string s("abcd12_efgh34");
//     std::regex e("abcd(\\d+)_efgh(\\d+)");
//     std::smatch sMatchResult;

//     if (std::regex_match(s, sMatchResult, e))
//     {
//     std::cout << "string Match:" << sMatchResult.size() << "\n";

//     for (size_t i = 0; i < sMatchResult.size(); ++i)
//     {
//     cout << sMatchResult[i] << " ";
//     }
//     }
//     cout << "\n";

//     std::cmatch cMatchResult;    // same as std::match_results<const char*> cm;
//     if (std::regex_match(s.c_str(), cMatchResult, e))
//     {
//     std::cout << "char* Match:" << cMatchResult.size() << "\n";

//     for (size_t i = 0; i < cMatchResult.size(); ++i)
//     {
//     cout << cMatchResult[i] << " ";
//     }
//     }

//     getchar();

//     std::cout << std::endl;
// }

// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// using namespace std;

// void divide(string s,vector<string> &v,regex reg)
// {
// 	smatch mResult;
// 	if (regex_search(s, mResult, reg)) {
// 		for (auto it = mResult.prefix().first; it != mResult.prefix().second; it++) {
// 			if (*it == ',')
// 				continue;
// 			v.push_back(string(1, *it));
// 		}
// 		v.push_back(string(mResult[0].first, mResult[0].second));
// 		divide(mResult.suffix(), v, reg);
// 	}
// 	else {
// 		for (auto it = s.begin(); it != s.end(); it++)
// 			v.push_back(string(1, *it));
// 	}
// }

// int main()
// {
// 	// string source;
// 	// vector<string> mid;
// 	// cin >> source;
// 	// regex reg("[a-zA-Z]+");
// 	// divide(source, mid, reg);


// 	// vector<string>::iterator it;
// 	// for (it = mid.begin(); it != mid.end(); it++) {
// 	// 	cout << *it << ' ';
// 	// }
// 	vector<int> v = vector<int>();
// 	v.push_back(1);
// 	cout << v.size() << endl;
// }

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
	vector<int> v = { 1, 20, 2, 6, 3, 7 };
	int key = 6;

	if (find(v.begin(), v.end(), key) != v.end())
		cout << "Element found";
	else
		cout << "Element not found";

	return 0;
}