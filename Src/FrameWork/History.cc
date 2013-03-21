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

Contributor(s): Timo Erkkilä.
*/

#include <algorithm>

#include "History.hh"
#include "HistoryNode.hh"
#include "error_handling.hh"


History::History() :
    
    history_nodes(),
    
    begin_cit( *this ),
    end_cit( *this ),

    rbegin_cit( *this, -1 ),
    rend_cit( *this, -1 )

{ }

History::~History() { }

const History&
History::operator=( const History& h )
{
    history_nodes = h.history_nodes;

    end_cit.MoveToNode( history_nodes.size() );
    rbegin_cit.MoveToNode( history_nodes.size() - 1 );

    return *this;
}

bool
History::empty() const { return history_nodes.empty(); }

void
History::push_back( const HistoryNode& n )
{
    history_nodes.push_back( n );
    ++end_cit;
    ++rbegin_cit;
}

const HistoryNode&
History::back() const
{
    check_claim( history_nodes.size(), "History::back(): "
                 "history_nodes.size() == 0" );
    return history_nodes.back();
}

HistoryNode&
History::back()
{
    check_claim( history_nodes.size(), "History::back(): "
                 "history_nodes.size() == 0" );
    return history_nodes.back();
}

void
History::Sort()
{
    unsigned i = 0;
    for ( ; i < history_nodes.size(); ++i )
    {
        history_nodes[ i ].SortRulesHistory();
    }
    
    if ( i >= 2 )
    {
        // Sorting to increasing order. The oldest history node will be
        //   the first and the latest the last.
        std::sort( history_nodes.begin(), history_nodes.end() );
    }

}

// Returns all nodes of the history tree in vector sorted so that
//   the first node of the vector has number 1 (no node number) and
//   the last node has the highest number.
const std::vector<HistoryNode>&
History::GiveNodes() const { return history_nodes; }

std::vector<HistoryNode>&
History::GiveNodes() { return history_nodes; }


// Adds a new parallel history to the current history.
const HistoryNode::Mark&
History::AddParallelHistory( const History& par_h )
{
    check_claim( history_nodes.size(), "History::AddParallelHistory(): "
                 "history_nodes.size() == 0" );
    check_claim( par_h.history_nodes.size(), "History::AddParallelHistory(): "
                 "par_h.history_nodes.size() == 0" );

    unsigned i = history_nodes.size();
    const unsigned addit = back().GiveMark().branch;

    history_nodes.insert( history_nodes.end(), par_h.history_nodes.begin(),
                    par_h.history_nodes.end() );

    // Renumbering the branch numbers of the added history nodes:
    for ( ; i < history_nodes.size(); ++i )
    {
        history_nodes[ i ].AddToBranchNumbers( addit );
        ++end_cit;
        ++rbegin_cit;
    }

    return back().GiveMark();
}


// const_iterator class:

// Sentinels: [rend, index 0][]...[][end, index size()-1]

History::const_iterator::const_iterator( const History& H, int index ) :
    
    history( &H ),
    history_nodes( &(H.GiveNodes()) ),
    i( index )

{ }

History::const_iterator::const_iterator( const const_iterator& cit ) :
    
    history( cit.history ),
    history_nodes( cit.history_nodes ),
    i( cit.i )

{ }
  
History::const_iterator::~const_iterator() { }

History::const_iterator&
History::const_iterator::operator=( const const_iterator& cit )
{    
    history = cit.history;
    history_nodes = cit.history_nodes;
    i = cit.i;

    return *this;
}

const HistoryNode&
History::const_iterator::operator*() const
{
    check_claim( i >= 0 && i < static_cast<int>( history_nodes->size() ),
                 "History::const_iterator::operator*() called when "
                 "the iterator pointed at nowhere." );
    return GiveNode();
}

const HistoryNode* const
History::const_iterator::operator->() const
{
    check_claim( i >= 0 && i < static_cast<int>( history_nodes->size() ),
                 "History::const_iterator::operator->() called when "
                 "the iterator pointed at nowhere." );
    return &GiveNode();
}

unsigned
History::const_iterator::numberOfChildren() const
{
    const int step = GiveNode().GiveMark().step;

    if ( !step )
    { return GiveNode().GiveMarksOfChildren().size(); }

    if ( step == -1 )
    { return 0; }

    return 1;
}

const HistoryNode&
History::const_iterator::MoveToChild( unsigned child_i )
{
    if ( GiveNode().GiveMark().step > 0 )
    { --i; }
    else
    {
        const HistoryNode::Mark& cMark =
            GiveNode().GiveMarksOfChildren()[ child_i ];
    
        for ( --i; !( GiveNode().GiveMark() == cMark); --i );
    }

    return GiveNode();
}
    
const HistoryNode&
History::const_iterator::MoveToParent()
{
    const HistoryNode::Mark& cMark = GiveNode().GiveMark();

    ++i;

    if ( GiveNode().GiveMark().branch == cMark.branch )
    { return GiveNode(); }

    while ( !GiveNode().isChild( cMark ) )
    { ++i; }

    return GiveNode();
}

bool
History::const_iterator::operator==( const const_iterator& comp ) const
{ return history == comp.history && i == comp.i; }
    
bool
History::const_iterator::operator!=( const const_iterator& comp ) const
{ return !( *this == comp ); }
