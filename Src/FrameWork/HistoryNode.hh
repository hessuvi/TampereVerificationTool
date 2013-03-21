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

// FILE_DES: HistoryNode.hh: FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: HistoryNode.hh 1.10 Fri, 27 Feb 2004 16:36:58 +0200 warp $
//
// HistoryNode class stores a history of one single file.
//

// $Log:$

#ifdef CIRC_HISTORYNODE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_HISTORYNODE_HH_
#define ONCE_HISTORYNODE_HH_
#define CIRC_HISTORYNODE_HH_


#include "config.hh"

#include <vector>
#include <string>
#include <iostream>
//#include <memory>


class History;


class HistoryNode
{
 public:

    HistoryNode( unsigned branchNumber = 0, int stepNumber = -1 );
    HistoryNode( const HistoryNode& );

    ~HistoryNode();

    const HistoryNode& operator=( const HistoryNode& );
    bool operator<( const HistoryNode& comp ) const
    {
        return mark.branch < comp.mark.branch ||
            mark.branch == comp.mark.branch && mark.step < comp.mark.step;
    }


    struct Mark
    {

     public:

        unsigned branch;
        int step; // -1 means: the node is a leaf node.

        Mark( unsigned branchNumber = 0, int stepNumber = -1 ) :
            branch( branchNumber ), step( stepNumber ) { }
        Mark( const Mark& m ) : branch( m.branch ), step( m.step ) { }

        Mark& operator=( const Mark& m )
        {
            branch = m.branch;
            step = m.step;
            return *this;
        }
        bool operator<( const Mark& comp ) const
        {
            return branch < comp.branch ||
                branch == comp.branch && step < comp.step;
        }
        bool operator==( const Mark& comp ) const
        { return branch == comp.branch && step == comp.step; }

    };


    const std::string& GiveFilename() const;
    const std::string& GiveCommandLine() const;
    const std::vector<std::string>& GiveCommentLines() const;

    const Mark& GiveMark() const;
    const std::vector<Mark>& GiveMarksOfChildren() const;

    bool isChild( const Mark& cm ) const
    {
        if ( cm.branch == mark.branch && cm.step == mark.step - 1 )
        { return true; }
        for ( unsigned i = 0; i < marks_of_children.size(); ++i )
        {
            if ( marks_of_children[ i ] == cm ) { return true; }
        }

        return false;
    }

    const History& GiveRulesHistory() const;



    // FOR THE INTERNAL USE OF THE LSTS I/O LIB:

    void SetFilename( const std::string& );
    void SetCommandLine( const std::string& );
    void AddCommentLine( const std::string& );

    void SetMark( const Mark& );
    void AddMarkOfChild( const Mark& );

    History& GiveRulesHistory();

    void Reset( unsigned branchNumber = 0, int stepNumber = -1 );

    void SetRulesHistory( const History& );
    void SortRulesHistory();
    void AddToBranchNumbers( unsigned addition );

 private:

    Mark mark;
    std::vector<Mark> marks_of_children;

    std::string filename;
    std::string command_line;
    std::vector<std::string> comment_lines;

    //mutable std::auto_ptr<History> rules_h;
    mutable History* rules_h;
};

// For printing marks to ostream object in form: <branch>[.<step>]
inline std::ostream& operator<< ( std::ostream& os, const HistoryNode::Mark m )
{
    os << m.branch;
    if ( m.step >= 0 )
    {
        os << '.' << m.step;
    }

    return os;
}

#undef CIRC_HISTORYNODE_HH_
#endif
