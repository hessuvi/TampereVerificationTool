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



#include <iostream.h>
#include "linealg.hh"

int
main()
{
  LineAlgorithm hor_beginnig(10,10,2,10);
  LineAlgorithm hor_end(2,10,2,10);
  for( ; hor_beginnig != hor_end ; ++ hor_beginnig)
    {
      Point cur = *hor_beginnig;
      cout << cur.xCoord() << "," << cur.yCoord() << endl;
    }

  LineAlgorithm ver_beginnig(10,10,10,20);
  LineAlgorithm ver_end(10,20,10,20);
  for( ; ver_beginnig != ver_end ; ++ ver_beginnig)
    {
      Point cur = *ver_beginnig;
      cout << cur.xCoord() << "," << cur.yCoord() << endl;
    }

  cout << endl;
  {
    LineAlgorithm diag_beginnig(10,10,7,6);
    LineAlgorithm diag_end(7,6,7,6);
    for( ; diag_beginnig != diag_end ; ++ diag_beginnig)
      {
        Point cur = *diag_beginnig;
        cout << cur.xCoord() << "," << cur.yCoord() << endl;
      }
    cout << endl;
  }
  {
    LineAlgorithm diag_beginnig(0,0,3,4);
    LineAlgorithm diag_end(3,4,3,4);
    for( ; diag_beginnig != diag_end ; ++ diag_beginnig)
      {
        Point cur = *diag_beginnig;
        cout << cur.xCoord() << "," << cur.yCoord() << endl;
      }
    cout << endl;
  }

}
