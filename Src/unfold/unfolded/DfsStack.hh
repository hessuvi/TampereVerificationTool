#ifndef DFS_STACK
#define DFS_STACK

#include "DynamicArray.hh"
#include "error_handling.hh"

#include <iostream>


struct ufdState
{
    unsigned fd_state_number;
    unsigned ufd_state_number;
    std::vector<int> variables;
    std::vector<bool> prev_props;

    ufdState* link;
};


class DfsStack
{
 public:
    DfsStack() : top( 0 ), last( &front ), recycled( 0 ) { }
    ~DfsStack() { check_claim( !top, "A bug caught in DfsStack::~DfsStack(): "
                               "stack is not empty!" ); }
    void get_new( ufdState*& ptr_ref )
    { if ( recycled ) { ptr_ref = recycled; recycled = recycled->link; }
      else { ptr_ref = &storage.push_back(); } }

    void recycle( ufdState* state )
    { state->link = recycled; recycled = state; }

    void push( ufdState* state ) { last->link = state; last = state; }
    void update() { last->link = top; top = front.link; last = &front; }

    void pop( ufdState*& ptr_ref ) { ptr_ref = top; top = top->link; }
    ufdState* give_top() { return top; }
    
 private:
    DfsStack( const DfsStack& src );
    DfsStack& operator=( const DfsStack& src );

    ufdState* top;
    ufdState* last;
    ufdState* recycled;

    ufdState front;
    
    DynamicArray<ufdState> storage;
};


#endif
