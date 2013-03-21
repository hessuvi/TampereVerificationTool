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



#include "nonsym.hh"




#include <iostream>
using namespace std;

void
IlNonSymmetricInitialSolution( int             N_I,
                               IlGrid          &grid_O,
                               IlLTS::StateIterator start_I,
                               IlLTS::StateIterator end_I)
{
  IlLTS::StateIterator ii=start_I;
  if( ! grid_O.placeState(*ii,0,0) )
    {
      
      
        std::cerr << "illus: Internal error at funtion "
             << "IlInitialSolution: "
             << __FILE__ << " "
             << __LINE__
             << std::endl;
      

          exit(1);
    }
  int cur_loc=2;
  
  for( ++ii ; (ii != end_I) && (cur_loc < N_I) ;  )
    {
      if( ! grid_O.placeState(*ii,cur_loc,0) )
        {
          
          
            std::cerr << "illus: Internal error at funtion "
                 << "IlInitialSolution: "
                 << __FILE__ << " "
                 << __LINE__
                 << std::endl;
          

              exit(1);
        }
      ++ii;
      if( ! grid_O.placeState(*ii,0,cur_loc) )
        {
          
          
            std::cerr << "illus: Internal error at funtion "
                 << "IlInitialSolution: "
                 << __FILE__ << " "
                 << __LINE__
                 << std::endl;
          

              exit(1);
        }
      ++ii;
      cur_loc += 2;
    }
  if( ii != end_I )
    {
      cur_loc = N_I / 2;
      if( ! grid_O.placeState(*ii,cur_loc,cur_loc) )
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



