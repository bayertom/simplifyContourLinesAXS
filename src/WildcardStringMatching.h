#ifndef WildcardStringMatching_H
#define WildcardStringMatching_H

#include <string>

#include "TVector2D.h"

class WildcardStringMatching
{
        public:
		static bool matchStringToPattern(std::string s, std::string p);

	private:
		static int findMatching(std::string& s, std::string& p, TVector2D <int>& dp, int n, int m);

};

#endif