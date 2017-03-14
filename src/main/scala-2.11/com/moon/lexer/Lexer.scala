package com.moon.lexer

/**
  * Created by lin on 3/14/17.
  */
trait Lexer {
  val EOF = -1
  val Method_Decl = 4
  val Arg_Decl = 5
  val Block = 6
  val Var_Decl = 7
  val Call = 8
  val Elist = 9
  val Expr = 10
  val ID = 11
  val T_Int = 12
  val Letter = 13
  val Ws = 14
  val Sl_Comment = 15
  // 左括号
  val Left_Bracket = 16
  // 右括号
  val Right_Bracket = 17
  // 逗号
  val Comma = 18
  val T_Float = 19
  val T_int = 20
  val T_void = 21
  val Left_Brace = 22
  val Right_Brace = 23
  val T_Equal = 24
  val T_Semicolon = 25
  val T_Return = 26
  val T_Add = 27
}
