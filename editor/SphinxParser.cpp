#include "TextParser.h"

#include <QtCore/QRegularExpression>
#include <cassert>

namespace Parser
{
bool isInComment( const QString& code, int maxLine, int maxColumn )
{
  return false;
}
bool isInList( const QString& code, int maxLine, int maxColumn )
{
  // and not comment

  // A text block which begins with a "*", "+", "-", "•", "‣", or "⁃",

  return false;
}
bool isLineWithOnlyWhitespaces( const QString& code )
{
  return false;
}
} // namespace Parser

namespace ParserHelper
{

using Info = std::tuple<int, int, int, int, int>;
enum INFO_PARTS
{
  COMMENT,
  DIRECTIVE,
  LIST,
  NUMBERED_LIST,
  AUTONUMBERED_LIST,
};

QString rStrip( const QString& command )
{
  QString str( command );

  while ( !str.isEmpty() && str.at( str.size() - 1 ).isSpace() )
  {
    str.chop( 1 );
  }

  return str;
}

bool isLineWithOnlyWhitespaces( const QString& code )
{
  bool whitespaces_only = false;
  if ( !code.isEmpty() )
  {
    whitespaces_only = ( code.startsWith( " " ) || code.startsWith( "\t" ) ) && ( code.trimmed().isEmpty() );
  }
  return whitespaces_only;
}

Info parseCode( const QString& code, int maxLine = -1, int maxColumn = -1 )
{
  assert( !code.isEmpty() );

  auto  lines           = code.split( QRegularExpression{R"-((\r\n?|\n))-"} );
  auto  roundBrackets   = 0;
  auto  braceBrackets   = 0;
  auto  curlyBrackets   = 0;
  auto  multilinestring = 0;
  auto  singlestring    = 0;
  auto  comment         = 0;
  QChar ignoreStringStart;
  QChar multiLineStart;
  auto  indent           = 0;
  bool  ignoreEveryThing = false;
  bool  ignoreColon      = false;

  maxLine = ( -1 == maxLine ) ? lines.count() : maxLine;

  for ( auto lineIdx = 0; lineIdx < maxLine; ++lineIdx )
  {
    auto line = ( !isLineWithOnlyWhitespaces( lines[lineIdx] ) ) ? rStrip( lines[lineIdx] ) : lines[lineIdx];
    if ( line.isEmpty() && ( 0 < indent ) && !ignoreEveryThing && lineIdx == ( maxLine - 1 ) )
    {
      indent = 0;
    }
    // reset ignore comment on every new line
    bool ignoreComment = false;
    auto lineIndent    = 0;

    auto maxCol =
      ( lineIdx < maxLine - 1 ) ? line.count() : ( -1 == maxColumn ) ? line.count() : ( ( maxColumn >= line.count() ) ? line.count() : maxColumn );

    for ( auto idx = 0; idx < maxCol; idx++ )
    {

      if ( multiLineStart.isNull() && ( '\'' == line[idx] || '"' == line[idx] ) )
      {
        if ( idx + 2 < maxCol )
        {
          if ( line[idx + 1] == line[idx] && line[idx + 2] == line[idx] )
          {
            multilinestring++;
            ignoreEveryThing = true;
            idx += 2;
            multiLineStart = line[idx];
          }
          else
          {
            multiLineStart = QChar::Null;
          }
        }
        else
        {
          multiLineStart = QChar::Null;
        }
      }
      else if ( !multiLineStart.isNull() )
      {
        if ( idx + 2 < maxCol )
        {
          if ( line[idx] == multiLineStart && line[idx + 1] == multiLineStart && line[idx + 2] == multiLineStart )
          {
            multilinestring--;
            ignoreEveryThing = false;
            multiLineStart   = QChar::Null;
            idx += 2;
          }
        }
      }
      if ( !( ignoreEveryThing ) )
      {
        if ( line[idx] == '\'' )
        {
          if ( ignoreStringStart.isNull() )
          {
            ignoreStringStart = '\'';
          }
          else if ( ignoreStringStart == line[idx] )
          {
            if ( 0 < idx && line[idx - 1] != '\\' )
            {
              ignoreStringStart = QChar::Null;
            }
          }
        }
        else if ( line[idx] == '"' )
        {
          if ( ignoreStringStart.isNull() )
          {
            ignoreStringStart = '"';
          }
          else if ( ignoreStringStart == line[idx] )
          {
            if ( 0 < idx && line[idx - 1] != '\\' )
            {
              ignoreStringStart = QChar::Null;
            }
          }
        }

        singlestring = ignoreStringStart.isNull() ? 0 : 1;

        if ( ignoreStringStart.isNull() )
        {
          if ( line[idx] == '#' )
          {
            ignoreComment = true;
          }

          comment = ignoreComment ? 1 : 0;

          if ( !ignoreComment )
          {
            if ( !ignoreColon && line[idx] == ':' )
            {
              lineIndent++;
            }
            if ( line[idx] == '(' )
            {
              roundBrackets++;
            }
            else if ( line[idx] == ')' )
            {
              roundBrackets--;
            }
            else if ( line[idx] == '{' )
            {
              curlyBrackets++;
              ignoreColon = true;
            }
            else if ( line[idx] == '}' )
            {
              curlyBrackets--;
              ignoreColon = false;
            }
            else if ( line[idx] == '[' )
            {
              braceBrackets++;
              ignoreColon = true;
            }
            else if ( line[idx] == ']' )
            {
              braceBrackets--;
              ignoreColon = false;
            }
          }
        }
      }
    }
    indent = ( 0 < lineIndent ) ? indent + 1 : indent;
  }

  return std::make_tuple( curlyBrackets, roundBrackets, indent, singlestring, comment );
}

bool codeValid( const Info& code_info )
{
  bool codeValid = true;
  //  auto braceBrackets = std::get<BRACE_BRACKETS>( code_info );
  //  auto curlyBrackets = std::get<CURLY_BRACKETS>( code_info );
  //  auto roundBrackets = std::get<ROUND_BRACKETS>( code_info );
  //  auto indent        = std::get<INDENT>( code_info );

  //  if ( ( 0 > braceBrackets ) || ( 0 > curlyBrackets ) || ( 0 > roundBrackets ) || ( 0 > indent ) )
  //  {
  //    // closing brackets more found than opening, also negative indent
  //    // this is a syntax error
  //    codeValid = false;
  //  }

  return codeValid;
}
} // namespace ParserHelper
