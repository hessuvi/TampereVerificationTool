/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Heikki Virtanen.
*/



#include "initial.hh"




#include <iostream>
using namespace std;



void
IlInitialSolution(int             N_I,
                  IlGrid          &grid_O,
                  IlLTS::StateIterator start_I,
                  IlLTS::StateIterator end_I)
{
  int curX;
  int curY;
  IlLTS::StateIterator ii;
  if( N_I < 4 )
    {
      
      
      for( ii = start_I, curX = 0 ;
           ii != end_I;
           ++ii, ++curX )
        {
          // cerr << "Olemme lisäämässä tilaa koordinaatteihin "
          // << curX << endl;
          if( ! grid_O.placeState(*ii,curX,curX) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              
      
              exit(1);
            }
        }
      

    }
  else
    {
      curX=0;
      curY=0;
  
      for( ii=start_I ;
           (ii != end_I) && (curX < N_I) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              

              exit(1);
            }
          curX += 4;
        }
      curY += curX - N_I + 1;
      curX  = N_I - 1;
      for( ;
           (ii != end_I) && (curY < N_I) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              

              exit(1);
            }
          curY += 4;
        }
      curX -= curY - N_I + 1;
      curY  = N_I -1 ;
      for( ;
           (ii != end_I) && (curX >= 0) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              

              exit(1);
            }
          curX -= 4;
        }
      curY += curX;   // -= 0-curX
      curX  = 0;
      for( ;
           (ii != end_I) && (curY > 0) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              

              exit(1);
            }
          curY -= 4;
        }
      if( ii != end_I )
        {
          if( ! grid_O.placeState(*ii,N_I/2,N_I/2) )
            {
              
              
                std::cerr << "illus: Internal error at funtion "
                     << "IlInitialSolution: "
                     << __FILE__ << " "
                     << __LINE__
                     << std::endl;
              

              exit(1);
            }
          ++ii;
        }
      if( ii != end_I )
        {
           cerr << "illus: Strange, too much states!" << endl;
           exit(1);
        }
    }
}                 




