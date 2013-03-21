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



#include "surround.hh"

#ifndef TOOLS_AUTO_VEC_HH
#include "auto_vec.hh"
#endif

#ifndef MAKEDEPEND
extern "C" {
#include <lp_solve/lpkit.h>
}
#include <cmath>
#endif







#ifndef M_PI
#define M_PI 3.14159265359
#endif

#include <cassert>

bool
IlTransitionRing::addLoop(IlLTS::Transition &trans, double minAngle)
{
  if( begin()==end() )
    {
      //% List is empty
      Item one(0.0, trans, trans.head());
      Item other(minAngle, trans, trans.tail());
      push_back(one);
      push_back(other);
    }
  else
    {
      Iterator current = end();
      current --;
      double lastAngle = (*current).getAngle() - 2*M_PI ;
      bool found = false;
      for( current = begin() ;
           current != end() ;
           ++current )
        {
          if( (*current).getAngle() - lastAngle >= 3*minAngle )
            {
              found = true;
              break;
            }
          else
            {
              lastAngle = (*current).getAngle();
            }
        }
      if( found )
        {
          Item one(lastAngle + minAngle, trans, trans.head());
          Item other(lastAngle + minAngle + minAngle, trans, trans.tail());
          current = insert(current, other);
          current = insert(current, one);
        }
      else
        {
          assert( found && "No room for loop. Sorry" );
        }
    }
  return( true );
}






bool
IlTransitionRing::optimize(double minAngle)
{


  bool     feasible;
  int      numOfEdges = size();
  // muuttuja(numOfEdges);
  double   jako = 2.0*M_PI/numOfEdges;
  feasible = jako > minAngle;

  auto_vec<double> alpha = new double [numOfEdges];
  auto_vec<int> lkm = new int [numOfEdges];
  auto_vec<ItemPtr> alkiot= new ItemPtr [numOfEdges];
//  list_item it;
  int gidx=0;
  int varidx=0;
  for(Iterator it=begin();
      it != end();
      ++it)
    {
      alkiot[gidx]=&(*it);
      if( (varidx > 0)
          && alkiot[gidx-1]->otherEnd() == alkiot[gidx]->otherEnd()
          && alkiot[gidx-1]->naturalAngle() == alkiot[gidx]->naturalAngle() )
        {
          lkm[varidx-1] += 1;
        }
      else
        {
          alpha[varidx]=alkiot[gidx]->naturalAngle();
          lkm[varidx]=1;
          ++varidx;
        }
      ++gidx;
    }


  if( !feasible )
    {
      minAngle = jako / 2.0;
    }


  // muuttuja(varidx);
  int numOfDesVar = 2 * varidx;
  int numOfVar = numOfDesVar;
  int numOfConstr = varidx;
  auto_vec<double> row = new double [numOfVar];



  if( numOfDesVar > 2 ) 
    {
      
      
      
        lprec *prob = make_lp(numOfConstr, numOfVar);
      
        for( int idx = 0 ; idx < numOfVar ; ++idx )
          {
            row[idx]=0.0;
          }
      
        // Adding constraints for decision variables
        row[0]=row[3]=1.0;
        row[1]=row[2]=-1.0;
        for( int idx = 0 ; idx < numOfConstr -2 ; ++idx )
          {
            add_constraint(prob, row.get()-1, LE ,
                           alpha[idx+1]
                            -alpha[idx]
                            -(lkm[idx+1]+lkm[idx])*minAngle/2.0 );
            for( int jdx = 2*idx + 5 ;
                 jdx >= 2*idx + 2;
                 --jdx )
              {
                row[jdx]=row[jdx-2];
                row[jdx-2]=0.0;
              }
          }
        add_constraint(prob, row.get()-1, LE ,
                       alpha[numOfConstr-1]-alpha[numOfConstr-2]
                        -(lkm[numOfConstr-1]+lkm[numOfConstr-2])*minAngle/2.0 );
        row[numOfDesVar-4]=row[numOfDesVar-3]=0.0;
        row[numOfDesVar-2]=row[1]=1.0;
        row[numOfDesVar-1]=row[0]=-1.0;
        add_constraint(prob, row.get()-1, LE ,
                       2*M_PI + alpha[0]-alpha[numOfConstr-1]
                         -(lkm[0]+lkm[numOfConstr-1])*minAngle/2.0 );
        
        // Adding objective function
        for( int idx = 0 ; idx < numOfVar ; ++idx )
          {
            row[idx] = 1.0; // alkiot[idx/2]->coff();
            // muuttuja(alkiot[idx/2]->coff());
          }
        set_obj_fn(prob,row.get()-1);
        set_minim(prob);
        // print_lp(prob);
      
      
        if( OPTIMAL != solve(prob) )
          {
            feasible = false;
            for( int idx = 0 ; idx < numOfVar ; ++idx )
              {
                row[idx]=0.0;
              }
          }
        else
          {
            
            
            double tarkistus=prob->best_solution[0];
            // muuttuja(tarkistus);
            for(int idx=0; idx<numOfVar; ++ idx)
              {
                row[idx]=prob->best_solution[prob->rows+idx+1];
                // muuttuja(row[idx]);
                tarkistus -= row[idx];
              }
            // muuttuja(tarkistus);
            assert((fabs(tarkistus) < 0.001 && "Muuttujista laskettu ratkaisu ei täsmää"));
            
      
          }
      
        delete_lp(prob);
      

    }
  else
    {
      if( numOfDesVar == 2 )
        {
          row[0]=row[1]=0.0;
        }
      else
        {
          return(feasible);
        }
      
    }


  varidx=0;
  double cang;
  for( int idx = 0 ; idx < numOfEdges ; ++varidx)
    {
      cang = alpha[varidx]+row[2*varidx]-row[2*varidx+1];
      cang -= (lkm[varidx]-1)*minAngle/2.0;
      for( int jdx = 0 ; jdx < lkm[varidx] ; ++jdx,++idx )
        {
          alkiot[idx]->setAngle(cang);
          cang += minAngle;
        }
    }
  
  return( feasible );
}




