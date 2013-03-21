/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#include "HistorySection.hh"
#include "History.hh"
#include "LSTS_File/Header.hh"
#include "HistoryNode.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"

#include "LogWrite.hh"
#include "error_handling.hh"


static void
ReadMark( HistoryNode::Mark& mark, ITokenStream& source )
{
    mark.branch = source.Get( TT::NUMBER ).numberData();

    if ( source.Peek().isPunctuation( "." ) )
    {
        source.Get();
        mark.step = source.Get( TT::NUMBER ).numberData();
        return;
    }
    
    mark.step = -1;
}

static void
ReadUsingMark( HistoryNode::Mark& mark,
                     ITokenStream& source )
{
    source.Get( TT::PUNCTUATION, "<|" );

    ReadMark( mark, source );

    if ( source.Peek().isPunctuation( ":" ) )
    {
        source.Get();
        ReadMark( mark, source );
    }

    source.Get( TT::PUNCTUATION, "|>" );
}

// Reads filename, command line and comment lines of a history node.
static void
ReadNodeData( HistoryNode& node, ITokenStream& source )
{
    if ( source.Peek().isString() )
    {
        node.SetFilename( source.Get( TT::STRING ).stringData() );
    }
    else
    {
        node.SetFilename( source.GiveFilename() );
    }
    
    if ( source.Peek().isString() )
    {
        node.SetCommandLine( source.Get( TT::STRING ).stringData() );
    }
    
    while ( source.Peek().isString() )
    {
        node.AddCommentLine( source.Get().stringData() );
    }
}

static void
ReadMarksOfChildren( HistoryNode& node, ITokenStream& source )
{
    source.Get( TT::IDENTIFIER, "from" );
    
    HistoryNode::Mark mark;
    ReadMark( mark, source );
    node.AddMarkOfChild( mark );
    
    while( source.Peek().isPunctuation( "&#||" ) )
    {
        source.Get();
        ReadMark( mark, source );
        node.AddMarkOfChild( mark );
    }
}

static void
ReadUsingNodes( HistoryNode& node, ITokenStream& source )
{
    while( source.Peek().isPunctuation( "<|" ) )
    {
        HistoryNode using_node;
        HistoryNode::Mark mark;
        
        ReadUsingMark( mark, source );
        using_node.SetMark( mark );
        
        if ( source.Peek().isIdentifier( "from" ) )
        {
            ReadMarksOfChildren( node, source );   
        }            

        ReadNodeData( using_node, source );

        node.GiveRulesHistory().push_back( using_node );
    }
    
}
    
static void
ReadNode( HistoryNode& node, ITokenStream& source )
{
    HistoryNode::Mark mark;
    bool rules_hist = false;
    
    if ( source.Peek().isNumber() )
    {
        ReadMark( mark, source );
    }

    node.SetMark( mark );

    
    if ( source.Peek().isIdentifier( "FROM" ) )
    {
        ReadMarksOfChildren( node, source );
    }            

    if ( source.Peek().isIdentifier( "Using" ) )
    {
        source.Get();
        ReadUsingMark( mark, source );
        rules_hist = true;
    }
    
    ReadNodeData( node, source );
    
    if ( rules_hist )
    {
        ReadUsingNodes( node, source );
    }

}
    
// Reads the whole history section and stores the data to the header.
//===========================================================================
//
void
iHistorySection::ReadSection( Header& )
{
    ITokenStream& source = its();

    source.SetPunctuation( ".#&#<|#|>#:#||" );

    // Inputting the history data:

    check_claim( source.GiveInStream(), "HistorySection::ReadSection: "
                 "source.GiveInStream() == 0 " );

    History& h = source.GiveInStream()->GiveHistory();

    if ( source.Peek().isString() )
    {
        // Single history node in simple format?
        HistoryNode node;
        ReadNode( node, source );
        h.push_back( node );
    }

    while ( source.Peek().isNumber() )
    {
        HistoryNode node;
       
        ReadNode( node, source );
       
        h.push_back( node );
    }

    h.Sort();
}



// *** The writing methods ***


bool
oHistorySection::doWeWriteSectionWithInit( Header& ) { return true; }

static void
WriteNN( HistoryNode::Mark mark, OTokenStream& o )
{
    o.PutNumber( mark.branch );
    
    if ( mark.step >= 0 )
    {
        o.spaceOff();
        o.PutPunctuation( "." );
        o.PutNumber( mark.step );
        o.spaceOn();
    }
}

static void
WriteMark( HistoryNode::Mark mark, OTokenStream& o,
                 const HistoryNode::Mark* host_nn )
{
    if ( host_nn )
    {
        o.PutPunctuation( "<|" );
        o.spaceOff();
        WriteNN( *host_nn, o );
        o.spaceOff();
        o.PutPunctuation( ":" );
        o.spaceOff();
    }

    WriteNN( mark, o );

    if ( host_nn )
    {
        o.spaceOff();
        o.PutPunctuation( "|>" );
        o.spaceOn();
    }
}

static void
WriteHistoryNode( const HistoryNode& node, OTokenStream& o,
                  const HistoryNode::Mark* host_nn = 0 )
{
    // Number of history node:
    WriteMark( node.GiveMark(), o, host_nn );
    
    if ( node.GiveMarksOfChildren().size() )
    {
        o.PutIdentifier( "From" );
        
        const std::vector<HistoryNode::Mark>& pcNns =
            node.GiveMarksOfChildren();
        
        WriteMark( pcNns[ 0 ], o, host_nn );
        
        for ( unsigned i = 1; i < pcNns.size(); ++i ) 
        {
            o.PutPunctuation( "&" );
            WriteMark( pcNns[ i ], o, host_nn );
        }

    }

    const History& rhist = node.GiveRulesHistory();

    if ( !rhist.empty() )
    {
        History::const_iterator rhi( rhist.rbegin() );
        const HistoryNode::Mark nn = node.GiveMark();
        o.Endl();
        o.PutIdentifier( "Using" );
        WriteMark( rhi->GiveMark(), o, &nn );
    }
    
    o.Endl();
    
    // Filename:
    o.PutString( node.GiveFilename() );
    o.Endl();

    // Command line:
    if ( node.GiveCommandLine().size() )
    {
        o.PutString( node.GiveCommandLine() );
        o.Endl();
    }

    // Comments:
    for ( unsigned i = 0; i < node.GiveCommentLines().size(); ++i )
    {
	o.PutString( node.GiveCommentLines()[i] );
        o.Endl();
    }

}

static void
WriteRulesHistory( const HistoryNode& node, OTokenStream& o )
{
    const History& rhist = node.GiveRulesHistory();
    for ( History::const_iterator rhi( rhist.rbegin() );
          rhi != rhist.rend();
          --rhi )
    {
        o.Endl();
        const HistoryNode::Mark mark_of_host = node.GiveMark();
        WriteHistoryNode( *rhi, o, &mark_of_host );
    }
}    

void
oHistorySection::WriteSection( Header& hd )
{
    WriteBeginSection();

    OTokenStream& o = ots();
    const OutStream& outs = *(o.GiveOutStream());

    HistoryNode root( 1 ); // = hd.GivePresentHistoryNode();
 
    root.SetFilename( outs.GiveFilename() );
    root.SetCommandLine( LogWrite::getDateTime() + " " +
                         outs.GiveCommandLine() );

    const std::vector<std::string>& comments = outs.GiveCommentLines();
    for ( unsigned i = 0; i < comments.size(); ++i )
    { root.AddCommentLine( comments[ i ] ); }

    History oHistory;

    std::vector<InStream*> inss;

    unsigned number_of_children = 0;


    for ( unsigned i = 0; i < outs.GiveInStreams().size(); ++i )
    {
        InStream* ip = outs.GiveInStreams()[i];

        if ( ip->GiveHistory().empty() )
        {
            HistoryNode node( 1 );
            node.SetFilename( ip->GiveFilename() );
            ip->GiveHistory().push_back( node );
        }

        const std::string& fileType = ip->GiveFileType();
        const unsigned ftSize = fileType.size();

        if ( ftSize >= 10 &&
             fileType.substr( ftSize - 10 ) == "RULES_FILE" )
        {
            root.SetRulesHistory( ip->GiveHistory() );
            continue;
        }

        inss.push_back( ip );
        ++number_of_children;
    }

    if ( number_of_children >= 1 )
    {
        oHistory = (*inss.begin())->GiveHistory();
    }

    if ( number_of_children >= 2 )
    {
        root.AddMarkOfChild( oHistory.back().GiveMark() );
        
        for ( unsigned i = 1; i < inss.size(); ++i )
        {
            unsigned ch = 0;
            for ( ; ch < i; ++ch )
            {
                // Now if we have copies of the same stream we won't
                //   dublicate them here:
                if ( inss[ ch ] == inss[ i ] )
                {
                    break;
                }
            }

            HistoryNode::Mark cmark;

            if ( ch < i )
            {
                cmark = root.GiveMarksOfChildren()[ ch ];
            }
            else
            {
                cmark =
                    oHistory.AddParallelHistory( inss[i]->GiveHistory() );
            }

            root.AddMarkOfChild( cmark );
        }
    }

    
    if ( number_of_children )
    {
        HistoryNode::Mark mark;
        mark = oHistory.back().GiveMark();

        if ( number_of_children >= 2 )
        {
            ++mark.branch;
            mark.step = 0;
        }
        else
        {
            ++mark.step;
            if ( !mark.step )
            {
                ++mark.step;
            }
        }
        
        root.SetMark( mark );
    }


    // Writing the present history node:
    WriteHistoryNode( root, o );

    o.PutWord( "\"" );
    o.spaceOff();
    o.PutWord( "States: " );
    o.PutNumber( hd.GiveStateCnt() );
    o.PutWord( ". Transitions: " );
    o.PutNumber( hd.GiveTransitionCnt() );
    o.PutWord( ".\"" );
    o.Endl();
    o.spaceOn();

    WriteRulesHistory( root, o );
  
 
    // Writing the rest of the nodes:
    for ( History::const_iterator hi( oHistory.rbegin() ); 
          hi != oHistory.rend();
          --hi )
    {
        o.Endl();
        WriteHistoryNode( *hi, o );
        WriteRulesHistory( *hi, o );
    }

    
    WriteEndSection();
}
