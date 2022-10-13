// Description: Number rounding

// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.


#ifndef Round_HPP
#define Round_HPP

template <typename T>
T Round::roundNumber ( const T num, const unsigned short dec_places )
{
        //Round number using selected decimal places
        T num_rounded = num;
        const long int divider = ( long int ) pow ( 10.0, dec_places );

        //Positive number
        if ( num > 0 )
        {
                num_rounded = ( floor ( divider * num + 0.5 ) ) / divider;
        }

        //Negative number
        else if ( num < 0 )
        {
                num_rounded = ( ceil ( divider * num - 0.5 ) ) / divider;
        }

        //Return rounded number
        return num_rounded;
}

#endif
