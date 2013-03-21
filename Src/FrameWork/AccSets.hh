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

Contributor(s): Juha Nieminen.
*/

#ifdef CIRC_ACCSETS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ACCSETS_HH_
#define ONCE_ACCSETS_HH_
#define CIRC_ACCSETS_HH_

#include "BitVector.hh"
#include "LSTS_File/AccSetsAP.hh"
#include "LSTS_File/Header.hh"

#include "error_handling.hh"

#ifndef MAKEDEPEND
#include <deque>
#include <list>
#endif


//========================================================================
// AccSets
//========================================================================
class AccSets
{
public:
    typedef std::list<BitVector>::const_iterator AccSetPtr;
    class action_const_iterator;

    inline AccSets();
    inline AccSets(unsigned maxVal); // Action range
    inline ~AccSets();

    inline void setMaxVal(unsigned maxVal); // Action range

    inline void createNewAccSet();
    inline void addActionToNewAccSet(unsigned actionNumber);
    inline void addNewAccSet();
    inline void doneAddingAccSets();

    inline void addAccSets(const AccSets& accsets);

    inline AccSetPtr firstAccSet() const;
    inline AccSetPtr endAccSet() const;
    inline action_const_iterator begin(AccSetPtr ptr) const;
    inline action_const_iterator end(AccSetPtr ptr) const;

    inline bool isMember(unsigned actionNumber, AccSetPtr ptr) const;

    inline unsigned setsAmount() const;

    inline bool isEmpty() const;

    inline BitVector getAllUsedActionsAsBitVector() const;

    inline bool operator<(const AccSets& rhs) const;
    inline bool operator==(const AccSets& rhs) const;
    inline bool operator!=(const AccSets& rhs) const;

    void writeAccSets(lsts_index_t stateNumber, iAccSetsAP& pipe);

    class action_const_iterator
    {
     public:
        inline action_const_iterator():
            accset(0), actionIndex(0), actionMaxVal(0) {}

        inline unsigned operator*() const;
        inline action_const_iterator& operator++();
        inline bool operator==(const action_const_iterator&) const;
        inline bool operator!=(const action_const_iterator&) const;

     private:
        friend class AccSets;
        inline action_const_iterator(AccSetPtr accset,
                                     unsigned actionMaxVal,
                                     unsigned index=0);

        inline void searchNextAction();

        const BitVector* accset;
        unsigned actionIndex;
        unsigned actionMaxVal;
    };

    template<typename InputIterator>
    inline AccSets(unsigned maxVal, InputIterator first, InputIterator last);



//========================================================================
private:
//========================================================================
    typedef std::list<BitVector> AccSetsCont;
    AccSetsCont sets;
    unsigned actionMaxVal;

#ifdef DEBUG
    bool actionMaxValHasBeenSet;
#endif

    // Disable copying:
    //AccSets (const AccSets&);
    //AccSets& operator=(const AccSets&);
};






//========================================================================
// AccSetsContainer
//========================================================================
class AccSetsContainer: public iAccSetsAP, public oAccSetsAP
{
 public:
    AccSetsContainer();
    AccSetsContainer(unsigned maxVal); // Action range

    ~AccSetsContainer();

    inline void setMaxVal(unsigned maxVal); // Action range

    // Returns a reference to the AccSets instance for the given state.
    // If it didn't exist, it is created.
    // setMaxVal() must be called before calling this method.
    inline AccSets& getAccSets(unsigned stateNumber);

    inline bool isInitialized() const;
    inline bool isEmpty() const;

//========================================================================
 protected:
    // Methods for reading:
    virtual void lsts_StartAccSets(Header&);
    virtual void lsts_StartAccSetsOfState(lsts_index_t state);
    virtual void lsts_StartSingleAccSet(lsts_index_t state);
    virtual void lsts_AccSetAction(lsts_index_t state, lsts_index_t action);
    virtual void lsts_EndSingleAccSet(lsts_index_t state);
    virtual void lsts_EndAccSetsOfState(lsts_index_t state);
    virtual void lsts_EndAccSets();

    // Methods for writing:
    virtual bool lsts_doWeWriteAccSets();
    virtual void lsts_WriteAccSets(iAccSetsAP& pipe);

 private:
    typedef std::deque<AccSets> AccSetsCont_t;
    AccSetsCont_t accsets;
    unsigned actionMaxVal;
    bool actionMaxValHasBeenSet;
};










//========================================================================
// Implementations
//========================================================================


// AccSets
//========================================================================

// action_const_iterator
// ---------------------
inline unsigned AccSets::action_const_iterator::operator*() const
{
#ifdef DEBUG
    if(actionIndex == actionMaxVal)
        check_claim(false,
                    "In AccSets::action_const_iterator::operator*(): "
                    "Tried to get value of end(), which should not be done.");
    check_claim(actionIndex < actionMaxVal,
                "In AccSets::action_const_iterator::operator*(): "
                "Tried to get the value of an iterator that is larger than "
                "the end() value, which should not be done.");
#endif

    return actionIndex+1;
}

inline void AccSets::action_const_iterator::searchNextAction()
{
    for(; actionIndex<actionMaxVal; actionIndex++)
    {
        if((*accset)[actionIndex]) return;
    }
}

inline AccSets::action_const_iterator&
AccSets::action_const_iterator::operator++()
{
#ifdef DEBUG
    warn_ifnot(actionIndex < actionMaxVal,
               "In AccSets::action_const_iterator::operator++(): "
               "Applied ++ to an iterator pointing to end().");
#endif

    ++actionIndex;
    searchNextAction();
    return *this;
}

inline bool
AccSets::action_const_iterator::operator==(const action_const_iterator& rhs)
    const
{
    return actionIndex==rhs.actionIndex && accset==rhs.accset;
}

inline bool
AccSets::action_const_iterator::operator!=(const action_const_iterator& rhs)
    const
{
    return actionIndex!=rhs.actionIndex || accset!=rhs.accset;
}

inline
AccSets::action_const_iterator::action_const_iterator
(AccSetPtr acceptanceSet, unsigned actionMaximumValue, unsigned index):
    accset(&(*acceptanceSet)), actionIndex(index),
    actionMaxVal(actionMaximumValue)
{
    searchNextAction();
}

// Constructors
//========================================================================
inline AccSets::AccSets():
    actionMaxVal(0)
#ifdef DEBUG
    , actionMaxValHasBeenSet(false)
#endif
{}

inline AccSets::AccSets(unsigned maxVal):
    actionMaxVal(maxVal)
#ifdef DEBUG
    , actionMaxValHasBeenSet(true)
#endif
{}

inline AccSets::~AccSets() {}

inline void AccSets::setMaxVal(unsigned val)
{
#ifdef DEBUG
    check_claim(sets.empty(),
                "In AccSets::setMaxVal(): "
                "Attempted to modify action max value after adding sets.");
    actionMaxValHasBeenSet = true;
#endif

    actionMaxVal = val;
}

// A special constructor intended for RO_AccSetsContainer
// ------------------------------------------------------
template<typename InputIterator>
inline AccSets::AccSets(unsigned maxVal,
                        InputIterator first, InputIterator last):
    actionMaxVal(maxVal)
#ifdef DEBUG
    , actionMaxValHasBeenSet(true)
#endif
{
    while(first != last)
    {
        sets.push_back(BitVector(maxVal, *first));
        ++first;
    }
}

// Creating a new acceptance set
//========================================================================
inline void AccSets::createNewAccSet()
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::createNewAccSet(): "
                "actionMaxVal has not been set.");
#endif

    sets.push_back(actionMaxVal);
}

inline void AccSets::addActionToNewAccSet(unsigned actionNumber)
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::addActionToNewAccSet(): "
                "actionMaxVal has not been set.");
    check_claim(actionNumber > 0 && actionNumber <= actionMaxVal,
                "In AccSets::addActionToNewAccSet(): "
                "Parameter (actionNumber) is not in the valid range.");
#endif

    sets.back().setBit(actionNumber-1);
}

inline void AccSets::addNewAccSet()
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::addNewAccSet(): "
                "actionMaxVal has not been set.");
#endif

    AccSetsCont::iterator newset = --sets.end();
    // Add a new set to the group of sets with the minimal sets rule
    for(AccSetsCont::iterator curr = sets.begin(); curr != newset;)
    {
        // If an existing set is a subset of the new set, the new set
        // is not added
        if(curr->isSubsetOf(*newset))
        {
            sets.erase(newset); break;
        }
        // If the new set is a subset of an existing set, the existing set
        // is removed
        if(newset->isSubsetOf(*curr))
        {
            curr = sets.erase(curr);
        }
        else ++curr;
    }
}

inline void AccSets::doneAddingAccSets()
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::doneAddingAccSets(): "
                "actionMaxVal has not been set.");
#endif

    sets.sort();
}

// Merging of two AccSets
//========================================================================
inline void AccSets::addAccSets(const AccSets& newsets)
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::addAccSets(): "
                "actionMaxVal has not been set.");
    check_claim(newsets.actionMaxValHasBeenSet,
                "In AccSets::addAccSets(): "
                "actionMaxVal of parameter has not been set.");
    check_claim(actionMaxVal == newsets.actionMaxVal,
                "In AccSets::addAccSets(): "
                "actionMaxVal of parameter is not the same as in *this.");
#endif

    for(AccSetsCont::const_iterator curr = newsets.sets.begin();
        curr != newsets.sets.end(); ++curr)
    {
        sets.push_back(*curr);
        addNewAccSet();
    }
}

// Reading methods
//========================================================================
inline AccSets::AccSetPtr AccSets::firstAccSet() const
{
    return sets.begin();
}

inline AccSets::AccSetPtr AccSets::endAccSet() const
{
    return sets.end();
}

inline AccSets::action_const_iterator AccSets::begin(AccSetPtr ptr) const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::begin(): "
                "actionMaxVal has not been set.");
    check_claim(ptr != endAccSet(),
                "In AccSets::begin(): "
                "Tried to get action_const_iterator from an endAccSet() "
                "iterator.");
#endif

    return action_const_iterator(ptr, actionMaxVal);
}

inline AccSets::action_const_iterator AccSets::end(AccSetPtr ptr) const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::end(): "
                "actionMaxVal has not been set.");
    check_claim(ptr != endAccSet(),
                "In AccSets::end(): "
                "Tried to get action_const_iterator from an endAccSet() "
                "iterator.");
#endif

    return action_const_iterator(ptr, actionMaxVal, actionMaxVal);
}


inline bool AccSets::isMember(unsigned actionNumber, AccSetPtr ptr) const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::isMember(): "
                "actionMaxVal has not been set.");
    check_claim(actionNumber > 0 && actionNumber <= actionMaxVal,
                "In AccSets::isMember(): "
                "Parameter (actionNumber) is not in the valid range.");
    check_claim(ptr != endAccSet(),
                "In AccSets::isMember(): "
                "Tried to use an endAccSet() iterator.");
#endif

    return (*ptr)[actionNumber-1];
}

inline bool AccSets::isEmpty() const
{
    return sets.empty();
}

inline unsigned AccSets::setsAmount() const
{
    return sets.size();
}

inline BitVector AccSets::getAllUsedActionsAsBitVector() const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::getAllUsedActionsAsBitVector(): "
                "actionMaxVal has not been set.");
#endif

    BitVector actions(actionMaxVal);
    for(AccSetPtr iter = firstAccSet(); iter != endAccSet(); ++iter)
    {
        actions |= *iter;
    }
    return actions;
}

inline bool AccSets::operator<(const AccSets& rhs) const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::operator<(): "
                "actionMaxVal has not been set.");
    check_claim(rhs.actionMaxValHasBeenSet,
                "In AccSets::operator<(): "
                "actionMaxVal of parameter has not been set.");
    check_claim(actionMaxVal == rhs.actionMaxVal,
                "In AccSets::operator<(): "
                "actionMaxVal of parameter is not the same as in *this.");
#endif

    if(sets.size() != rhs.sets.size())
        return sets.size() < rhs.sets.size();

    for(AccSetsCont::const_iterator iter1=sets.begin(), iter2=rhs.sets.begin();
        iter1 != sets.end(); ++iter1, ++iter2)
    {
        if((*iter1) < (*iter2)) return true;
        if((*iter2) < (*iter1)) return false;
    }
    return false;
}

inline bool AccSets::operator==(const AccSets& rhs) const
{
#ifdef DEBUG
    check_claim(actionMaxValHasBeenSet,
                "In AccSets::operator<(): "
                "actionMaxVal has not been set.");
    check_claim(rhs.actionMaxValHasBeenSet,
                "In AccSets::operator<(): "
                "actionMaxVal of parameter has not been set.");
    check_claim(actionMaxVal == rhs.actionMaxVal,
                "In AccSets::operator<(): "
                "actionMaxVal of parameter is not the same as in *this.");
#endif

    if(sets.size() != rhs.sets.size()) return false;

    for(AccSetsCont::const_iterator iter1=sets.begin(), iter2=rhs.sets.begin();
        iter1 != sets.end(); ++iter1, ++iter2)
    {
        if((*iter2) != (*iter1)) return false;
    }
    return true;
}

inline bool AccSets::operator!=(const AccSets& rhs) const
{
    return !((*this) == rhs);
}



// AccSetsContainer
//========================================================================
inline bool AccSetsContainer::isInitialized() const
{
    return actionMaxValHasBeenSet;
}

inline bool AccSetsContainer::isEmpty() const
{
    return accsets.size() == 0;
}

inline void AccSetsContainer::setMaxVal(unsigned maxVal)
{
    check_claim(!actionMaxValHasBeenSet,
                "In AccSetsContainer::setMaxVal(): Attempt to set action max "
                "value more than once.");

    actionMaxVal = maxVal;
    actionMaxValHasBeenSet = true;
    accsets[0].setMaxVal(maxVal);
}

inline AccSets& AccSetsContainer::getAccSets(unsigned stateNumber)
{
    check_claim(actionMaxValHasBeenSet,
                "In AccSetsContainer::getAccSets(): "
                "Action max value has not been initialized.");

    if(stateNumber >= accsets.size())
    {
        accsets.resize(stateNumber+1, AccSets(actionMaxVal));
    }

    return accsets[stateNumber];
}


#undef CIRC_ACCSETS_HH_
#endif
