#include <string.h>
#include "WildcardStringMatching.h"

bool WildcardStringMatching::matchStringToPattern(std::string s, std::string p)
{
        //Match input string to a pattern
        TVector2D<int> dp(s.size() + 1, TVector<int>(p.size() + 1, -1));
        
        return dp[s.size()][p.size()] = findMatching(s, p, dp, s.size() - 1, p.size() - 1);
}

int WildcardStringMatching::findMatching(std::string &s, std::string &p, TVector2D <int> &dp, int n, int m)
{
        //Return 1 if n and m are negative
        if (n < 0 && m < 0)
                return 1;

        //Return 0 if m is negative
        if (m < 0)
                return 0;

        //Return n if n is negative
        if (n < 0)
        {
                // while m is positve
                while (m >= 0)
                {
                        if (p[m] != '*')
                                return 0;
                        m--;
                }

                return 1;
        }

        //If dp state is not visited
        if (dp[n][m] == -1)
        {
                if (p[m] == '*')
                {
                        return dp[n][m] = findMatching(s, p, dp, n - 1, m) || findMatching(s, p, dp, n, m - 1);
                }

                else
                {
                        if (p[m] != s[n] && p[m] != '?')
                                return dp[n][m] = 0;

                        else
                                return dp[n][m] = findMatching(s, p, dp, n - 1, m - 1);
                }
        }

        // return dp[n][m] if dp state is previsited
        return dp[n][m];
}