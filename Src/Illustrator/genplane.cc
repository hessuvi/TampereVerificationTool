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



#include <iostream>
#include <cstdlib>
using namespace std;

static void writeHeader(int N, int M);
static void writeTransitions(int N, int M);

int
main(int argc, char **argv)
{
  if( argc < 3 )
    {
      cerr << "Usage: " << argv[0] << " <N> <M>" << endl;
      cerr << "Usage: " << argv[0] << " N > 2, M > 2" << endl;
      return( 1 );
    }
  int N = atoi(argv[1]);
  int M = atoi(argv[2]);
  if( (N < 3) || (M < 3) )
    {
      cerr << "Usage: " << argv[0] << " <N> <M>" << endl;
      cerr << "Usage: " << argv[0] << " N > 2, M > 2" << endl;
      return( 1 );
    }
  writeHeader(N,M);
  writeTransitions(N,M);
  return( 0 );
}

static void
writeHeader(int N, int M)
{
  cout << "lts" << endl;
  cout << "semantics cffd" << endl;
  cout << "state_cnt " << N*M << endl;
  cout << "action_rng 1" << endl;
  cout << "transit_cnt " << 4*N*M -3*N -3*M +2 << endl;
  cout << "initial_state 1" << endl;
  cout << "actions" << endl;
  cout << "0 |tau|" << endl;
  cout << "1 !a!" << endl;
  cout << "end_actions" << endl;
}

static void
writeTransitions(int N, int M)
{
  cout << "transitions" << endl;
  for( int jj = 1 ; jj < M ; ++jj )
    {
      for( int ii = 1 ; ii < N ; ++ii )
        {
          int se = jj*N+ii;
          int sw = se + 1;
          int ne = se - N;
          int nw = ne + 1;
          cout << ne << " " << nw << " 1 ;" << endl;
          cout << ne << " " << se << " 1 ;" << endl;
          cout << ne << " " << sw << " 1 ;" << endl;
          cout << nw << " " << se << " 1 ;" << endl;
          if( ii == N-1 )
            {
              cout << nw << " " << sw << " 1 ;" << endl;
            }
          if( jj == M-1 )
            {
              cout << se << " " << sw << " 1 ;" << endl;
            }
        }
    }
  cout << "end_transitions" << endl;
}

