/*
    LexerStage.hpp
    ==============
        Enum class LexerStage implementation.
*/

#pragma once

namespace CMM
{

////////////////////////////////////////////////////////////////////////////////
// Enum Class LexerStage
////////////////////////////////////////////////////////////////////////////////

enum class LexerStage
{
    Start,
    InId,
    InNumber,
    InDivide,
    InComment,
    EndComment,
    InLess,
    InGreater,
    InAssign,
    InNot,
    Done
};

} // End of namespace CMM