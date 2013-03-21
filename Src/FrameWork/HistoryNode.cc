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

// FILE_DES: HistoryNode.cc: FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
"Module version: $Id: HistoryNode.cc 1.13 Fri, 27 Feb 2004 16:36:58 +0200 warp $";

// 
// The implementation of the HistoryNode class.
//

// $Log:$


#include "HistoryNode.hh"
#include "History.hh"

#include <algorithm>

using std::sort;
using std::vector;
using std::string;

void
HistoryNode::Reset( unsigned branchNumber, int stepNumber )
{
    mark.branch = branchNumber;
    mark.step = stepNumber;

    marks_of_children.clear();

    filename = "<no filename given>";
    command_line = "<no command line given>";
    comment_lines.clear();

    /*if ( rules_h.get() )
    {
        delete rules_h.get();
        rules_h.release();
        }*/

    if ( rules_h )
    {
        delete rules_h;
        rules_h = 0;
    }

}


HistoryNode::HistoryNode( unsigned branchNumber, int stepNumber ) :
    
    rules_h( 0 )

{ Reset( branchNumber, stepNumber ); }

/*static inline History&
checkRulesHistory( std::auto_ptr<History>& History* apt )
{
    if ( !apt.get() )
    {
        std::auto_ptr<History> p(new History);
        apt = p;
        }

    if ( !apt )
    {
        apt = new History;
    }

    return *apt;
}*/

HistoryNode::HistoryNode( const HistoryNode& hn ) : rules_h( 0 )
{
    *this = hn;
}

const HistoryNode&
HistoryNode::operator=( const HistoryNode& hn )
{
    mark = hn.mark;
    marks_of_children = hn.marks_of_children;

    filename = hn.filename;
    command_line = hn.command_line;
    comment_lines = hn.comment_lines;

    if ( hn.rules_h )
    {
        if ( !rules_h ) rules_h = new History;
        *rules_h = *hn.rules_h;
    }
    else
    {
        if ( rules_h ) delete rules_h;
        rules_h = 0;
    }

    return *this;
}

HistoryNode::~HistoryNode() { Reset(); }


void
HistoryNode::SetFilename( const string& newFilename )
{ filename = newFilename; }

void
HistoryNode::SetCommandLine( const string& newCommandLine )
{ command_line = newCommandLine; }

void
HistoryNode::AddCommentLine( const string& line )
{ comment_lines.push_back( line ); }


void
HistoryNode::SetMark( const Mark& m )
{ mark = m; }

const HistoryNode::Mark&
HistoryNode::GiveMark() const { return mark; }

void
HistoryNode::AddMarkOfChild( const Mark& nm )
{ marks_of_children.push_back( nm ); }

const vector<HistoryNode::Mark>&
HistoryNode::GiveMarksOfChildren() const { return marks_of_children; }

void
HistoryNode::AddToBranchNumbers( unsigned addition )
{
    mark.branch += addition;

    for ( unsigned i = 0; i < marks_of_children.size(); ++i )
    {
        marks_of_children[ i ].branch += addition;
    }
}

void
HistoryNode::SortRulesHistory()
{
    if ( !rules_h )
    {
        return;
    }
    rules_h->Sort();
}


const string&
HistoryNode::GiveFilename() const { return filename; }

const string&
HistoryNode::GiveCommandLine() const { return command_line; }

const vector<string>&
HistoryNode::GiveCommentLines() const { return comment_lines; }


void
HistoryNode::SetRulesHistory( const History& h )
{
    if ( !rules_h ) rules_h = new History;
    *rules_h = h;
}

History&
HistoryNode::GiveRulesHistory()
{
    if ( !rules_h ) rules_h = new History;
    return *rules_h;
}

const History&
HistoryNode::GiveRulesHistory() const
{
    if ( !rules_h ) rules_h = new History;
    return *rules_h;
}

