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



#include "genetic.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <cstdlib>
#include <cmath>
#endif




class IlFitnessFunctionBase
{
public:
  virtual ~IlFitnessFunctionBase() {};
  virtual double operator () (double x) const = 0;
};

class IlConnectedFitness: public IlFitnessFunctionBase
{
double d_multiple;
public:
  IlConnectedFitness( int multiple ):d_multiple(multiple) {};
  double operator () (double x) const;
};

class IlNotConnectedFitness: public IlFitnessFunctionBase
{
public:
  double operator () (double x) const;
};


double
IlConnectedFitness::operator () (double x) const
{
  double xMP2 = (x-1.0);
  xMP2 *= xMP2;
  xMP2 += 1.0;
  return( 0.1 * d_multiple * x / xMP2 );
}

double
IlNotConnectedFitness::operator () (double x) const
{
  if( fabs(x) < 1.0 )
    {
      return x*x;
    }
  else
    {
      return 1.0;
    }
  //  x -= 0.5;
  //  x = exp(-x) + 1.0;
  //  return( 1.0 / x );
}

class IlFitnessFunctionItem
{
  
  int d_multiple;
  IlFitnessFunctionBase *d_func;

public:
  IlFitnessFunctionItem():d_multiple(0),d_func(0) {};

  int operator ++ () {return (++d_multiple);};
  int operator ++ (int) {return (d_multiple++);};

  void make()
    {
      if( d_func )
        {
          delete d_func;
        }
      if( d_multiple )
        {
          d_func = new IlConnectedFitness(d_multiple);
        } else {
        d_func = new IlNotConnectedFitness;
        }
    };
  double operator () (double x) const {return( (*d_func)(x) ); };
};


class Chromosome: public vector<double>
{
  typedef vector<double> Ancestor;
public:
  Chromosome(long size):Ancestor(2*size) {};

  void crossOver( double lambda,
                  const Chromosome &mom, const Chromosome &dad );
  void mutate(double amplitude = 2.0);
  void scale(double factor);
};

#include "DRand48.hh"

void
Chromosome::mutate(double amplitude)
{
  for( iterator idx=begin() ; idx != end() ; ++idx)
    {
      (*idx) += amplitude * ( DRand48() - 0.49999999);
    }
}

void
Chromosome::scale(double factor)
{
  for( iterator idx=begin() ; idx != end() ; ++idx)
    {
      double demi = 1 + exp(-(*idx)/factor);
      (*idx) = factor / demi ;
    }
}

#include <cassert>

void
Chromosome::crossOver( double lambda,
                       const Chromosome &mom, const Chromosome &dad )
{
  assert( (mom.size() == dad.size() && size() == mom.size() )
          && "WHOOOW!! Variable length cromosomes" );
  const_iterator idxMom;
  const_iterator idxDad;
  iterator idxMe;
  for( idxMe=begin(), idxMom=mom.begin(), idxDad=dad.begin() ;
       idxMe != end() ;
       ++idxMe, ++idxMom, ++idxDad )
    {
      *idxMe = *idxMom * lambda + *idxDad * (1.0 - lambda ) ;
    }
}

inline
static long
aritmSeries(long i)
{
  return( (i*i - i) / 2 );
}


class IlFitnessFunction: public vector<IlFitnessFunctionItem>
{
  typedef vector<IlFitnessFunctionItem> Ancestor;
public:
  IlFitnessFunction(long stateCnt): Ancestor(aritmSeries(stateCnt)) {};

  void addTransition(long xidx, long yidx);
  void make();
  double operator () (const Chromosome &chrom) const;
private:
  double apply(double x, long xidx, long yidx) const;

};

double 
IlFitnessFunction::apply(double x, long xidx, long yidx) const
{
  long idx = aritmSeries(xidx) + yidx ;
  return( ((*this)[idx])(x) );
}


inline
static double
localMaxNorm(double x1, double y1,
             double x2, double y2)
{
  return( max( fabs(x1-x2),fabs(y1-y2)));
}

double
IlFitnessFunction::operator () (const Chromosome &chrom) const
{
  long var_size = chrom.size() / 2;
  long fsize = 2*size();
  fsize += var_size;
  fsize -= var_size*var_size;
  assert( 0==fsize && "Not chromosome for this fitness function" );

  double retVal = 0.0;
  for( long xidx = 1 ; xidx < var_size ; ++xidx )
    {
      for( long yidx = 0; yidx < xidx ; ++yidx)
        {
          long vidx = xidx+xidx;
          long uidx = yidx+yidx;
          retVal += apply(localMaxNorm(chrom[vidx],chrom[vidx+1],
                                       chrom[uidx],chrom[uidx+1]),
                          xidx, yidx);
          
        }
    }
  return( retVal );
}

void
IlFitnessFunction::make()
{
  for( iterator idx=begin(); idx != end() ; ++idx )
    {
      idx->make();
    }
}

void
IlFitnessFunction::addTransition(long xidx, long yidx)
{
  if( xidx == yidx ) return;
  if( xidx < yidx )
    {
      addTransition(yidx,xidx);
    }
  else
    {
      long idx = (xidx * xidx - xidx ) / 2  + yidx ;
      ((*this)[idx])++;
    }
}



template<class TYPE>
inline static void
localSwap(TYPE &x, TYPE &y)
{
  TYPE tmp = x;
  x=y;
  y=tmp;
}


template <class TYPE>
inline static void
localSelectBestOutThree(TYPE *objs, double *rank)
{
  int idx = 0;
  if( rank[1] > rank[2] )
    {
      idx = 1;
    }
  else
    {
      idx = 2;
    }
  if( rank[0] > rank[idx] )
    {
      return;
    }
  else
    {
      localSwap(objs[0],objs[idx]);
      localSwap(rank[0],rank[idx]);
    }
}

template <class TYPE>
inline static void
localSelectWorstOutThree(TYPE *objs, double *rank)
{
  int idx = 0;
  if( rank[1] < rank[2] )
    {
      idx = 1;
    }
  else
    {
      idx = 2;
    }
  if( rank[0] < rank[idx] )
    {
      return;
    }
  else
    {
      localSwap(objs[0],objs[idx]);
      localSwap(rank[0],rank[idx]);
    }
}


void
IlGeneticAlgorithForCoordinates(IlLTS::StateIterator statesBeg_I,
                                IlLTS::StateIterator statesEnd_I,
                                IlLTS::TransitionIterator start_I,
                                IlLTS::TransitionIterator end_I)
{
  typedef map<state_t,long> IndexMap;
  IndexMap indexMap;

  // Let's make index from states
  long idxI;
  IlLTS::StateIterator  idxS;
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, ++idxI )
    {
      indexMap.insert(make_pair(idxS->name(),idxI));
    }
  assert( static_cast<size_t>(idxI) == indexMap.size()
          && "WHOOW !! Duplicate state names" );

  // Add transitions to fitness function
  IlFitnessFunction fitness(indexMap.size());
  long transCnt = 0;
  for( IlLTS::TransitionIterator tidx = start_I ;
       tidx != end_I ;
       ++tidx )
    {
      IndexMap::iterator uidx = indexMap.find(tidx->tail().name());
      IndexMap::iterator vidx = indexMap.find(tidx->head().name());
      assert( uidx != indexMap.end() && vidx != indexMap.end()
              && "Names of endpoints are not in index !");
      if( uidx != vidx )
        {
          fitness.addTransition(uidx->second,vidx->second);
          ++transCnt;
        }
    }
  // Finalize fitness function
  fitness.make();


  // This is microbial genetic algorithm
  // Population consist of three individuals
  Chromosome *population[3];
  Chromosome *child[3];
  double     popFitness[3];
  double     childFitness[3];
  population[0] = new Chromosome(indexMap.size());
#if 0
  // Get solution from input to chromosome
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, idxI+=2 )
    {
      (*(population[0]))[idxI] = idxS->coords().xCoord();
      (*(population[0]))[idxI+1] = idxS->coords().yCoord();
    }
#endif
  population[1] = new Chromosome(*(population[0]));
  population[2] = new Chromosome(*(population[0]));
  child[0]      = new Chromosome(indexMap.size());
  child[1]      = new Chromosome(indexMap.size());
  child[2]      = new Chromosome(indexMap.size());


  // Initial pupulation is created by mutating individuals several time
  // double delta = sqrt(indexMap.size()) / indexMap.size();
  for( int cnt = 0; cnt < 0 /* indexMap.size() / 2 */ ; ++cnt )
    {
      population[0]->mutate(0.1);
      population[1]->mutate(-0.5);
      population[2]->mutate(0.5);
    }
  popFitness[0] = fitness(*(population[0]));
  popFitness[1] = fitness(*(population[1]));
  popFitness[2] = fitness(*(population[2]));

  localSelectBestOutThree(population,popFitness);

  
  // counter counts non-profitable modifications. 
  for( int counter=0; counter < 3000 ; ++counter )
    {
      child[0]->crossOver(1.5*DRand48()-0.249999999999,
                       *(population[0]),*(population[1]));
      child[1]->crossOver(1.5*DRand48()-0.249999999999,
                       *(population[1]),*(population[2]));
      child[2]->crossOver(1.5*DRand48()-0.249999999999,
                       *(population[2]),*(population[0]));
      if( DRand48() < 0.8 )
        {
          child[0]->mutate(0.1);
        }
      if( DRand48() < 0.8 )
        {
          child[1]->mutate(0.2);
        }
      if( DRand48() < 0.8 )
        {
          child[2]->mutate(0.3);
        }
      childFitness[0]= fitness(*(child[0]));
      childFitness[1]= fitness(*(child[1]));
      childFitness[2]= fitness(*(child[2]));

      localSelectWorstOutThree(child,childFitness);

      localSwap( child[1],population[1]);
      localSwap( child[2],population[2]);
      localSwap( childFitness[1],popFitness[1]);
      localSwap( childFitness[2],popFitness[2]);

      double oldBest = popFitness[0];
      localSelectBestOutThree(population,popFitness);

      if( oldBest < popFitness[0] )
        {
          if( popFitness[0] - oldBest > 0.001 )
            {
              cerr << "Parannus löytyi " << popFitness[0]
                   << " " << counter << "                                 \r"
                   << flush ;
            }
          else
            {
              cerr << counter << "   \r" << flush;
            }
          counter = 0;
        }
    }

  cerr << endl;
  // Return solution from best chromosome
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, idxI+=2 )
    {
      idxS->coords() = Point((*(population[0]))[idxI],
                             (*(population[0]))[idxI+1]);
    }
  
}


