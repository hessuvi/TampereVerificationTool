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

// FILE_DES: History.hh: FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: History.hh 1.1.2.5 Fri, 27 Feb 2004 16:36:58 +0200 warp $
//
//

// $Log: History.hh,v $
// Revision 1.1  1999/08/16 04:41:02  timoe
// Initial revision
//

#ifdef CIRC_HISTORY_HH_
#error "Include recursion"
#endif

#ifndef ONCE_HISTORY_HH_
#define ONCE_HISTORY_HH_
#define CIRC_HISTORY_HH_


#include "HistoryNode.hh"

#include <string>
#include <vector>


class History
{

 public:

    History();
    ~History();

    const History& operator=( const History& );

    typedef std::vector<HistoryNode> history_nodes_t;

    class const_iterator
    {

     public:

        const_iterator( const History&, int index = 0 );
        const_iterator( const const_iterator& );
        ~const_iterator();

        const_iterator& operator=( const const_iterator& );

        const HistoryNode& operator*() const;
        const HistoryNode* const operator->() const;

        const const_iterator& operator++() { ++i; return *this; }
        const const_iterator& operator--() { --i; return *this; }

        const const_iterator& MoveToNode( int nodeIndex )
        { i = nodeIndex; return *this; }

        unsigned numberOfChildren() const;
        const HistoryNode& MoveToChild( unsigned childIndex = 0 );
        const HistoryNode& MoveToParent();

        bool operator==( const const_iterator& ) const;
        bool operator!=( const const_iterator& ) const;

     private:

        const HistoryNode& GiveNode() const { return (*history_nodes)[ i ]; }

        const History* history;
        const history_nodes_t* history_nodes;
        int i;

    };


    bool empty() const;

    const const_iterator& begin() const { return begin_cit; }
    const const_iterator& end() const { return end_cit; }

    const const_iterator& rbegin() const { return rbegin_cit; }
    const const_iterator& rend() const{ return rend_cit; }

    void push_back( const HistoryNode& );
    const HistoryNode& back() const;
    HistoryNode& back();



    // FOR THE INTERNAL USE OF THE LSTS I/O LIB:

    const history_nodes_t& GiveNodes() const;
    history_nodes_t& GiveNodes();

    void Sort();
    const HistoryNode::Mark& AddParallelHistory( const History& par_h );

 private:

    History( const History& ); // copy constructor is not in use.

    std::vector<HistoryNode> history_nodes;

    const const_iterator begin_cit;
    const_iterator end_cit;

    const_iterator rbegin_cit;
    const const_iterator rend_cit;

};


#undef CIRC_HISTORY_HH_
#endif
