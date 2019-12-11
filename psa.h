//
// Created by Samuel Spišák (xspisa02) on 25/11/2019.
//

#ifndef PROJEKTIFJ19_PSA_H
#define PROJEKTIFJ19_PSA_H

#include "token.h"
#include <stdbool.h>

typedef enum
{
    L,    /// < LEFT
    R,    /// > RIGHT
    E,    /// = EQUAL
    N     /// # ERROR
} PSA_operations_enum;

/**
 * @enum pravidla PSA
 */
typedef enum
{
    E_EQ_E,		    /// E -> E = E
    E_NEQ_E,		/// E -> E <> E
    E_LEQ_E,		/// E -> E <= E
    E_MEQ_E,		/// E -> E => E
    E_LESS_E,		/// E -> E < E
    E_MORE_E,		/// E -> E > E
    E_PLUS_E,		/// E -> E + E
    E_MINUS_E,	    /// E -> E - E
    E_IDIV_E,		/// E -> E // E
    E_MUL_E,		/// E -> E * E
    E_DIV_E,		/// E -> E / E
    LBR_E_RBR,		/// E -> (E)
    OPERAND,		/// E -> i
    INVALID_RULE	/// rule doesn't exist
} PSA_rules_enum;

/**
 * @enum indexy PSA tabulky.
 */
typedef enum
{
    T_PLUS_MINUS,		///  +-
    T_MUL_DIV,		    ///  */
    T_IDIV,			    ///  //
    T_RELATION,		    ///  r
    T_LEFT_BRACKET,	    ///  (
    T_RIGHT_BRACKET,	///  )
    T_VALUE,			///  i
    T_DOLLAR			///  $

} PSA_table_index_enum;

/**
 * @enum symboly PSA
 */
typedef enum
{
    PLUS,			/// +
    MINUS,			/// -
    MUL,			/// *
    DIV,			/// /
    IDIV,			/// //
    EQ,				/// =
    NEQ,			/// <>
    LEQ,			/// <=
    MEQ,			/// >=
    LESS,			/// <
    MORE,			/// >
    BRACKET_LEFT,	/// (
    BRACKET_RIGHT,	/// )
    IDENTIFIER,		/// ID
    INT_N,		    /// int
    DOUBLE_N,     	/// double
    STRING,			/// string
    NONE,           /// "None" special value
    DOLLAR		    /// $
} PSA_symbol_enum;


/**
 * analyzuje <expression> v LL gramatike
 *
 * @param current_token - aktualny token v scanneri
 * @param local - scope
 */
int expression(t_token * current_token, bool local, t_token *src_token);

#endif //PROJEKTIFJ19_PSA_H
