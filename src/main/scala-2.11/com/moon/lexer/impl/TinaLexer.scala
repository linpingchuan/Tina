package com.moon.lexer.impl

import com.moon.config.AppConfig
import com.moon.lexer.Lexer
import com.moon.state.impl.RecognizerSharedState
import com.moon.token.TinaToken

/**
  * Created by lin on 3/14/17.
  */
case class TinaLexer(src:String,state:RecognizerSharedState) extends Lexer{
  def this(str:String){
    this(str,new RecognizerSharedState)
  }

  /**
    * 从左括号开始,匹配左括号，并将状态设置为左括号
    */
  def startBracket(): Unit ={
    matchSymbol("(")
    state.tinaType=AppConfig.Left_Bracket
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 从右括号结束，匹配右括号，并将状态设置为右括号
    */
  def endBracket(): Unit ={
    matchSymbol(")")
    state.tinaType=AppConfig.Right_Bracket
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配逗号
    */
  def matchComma(): Unit ={
    matchSymbol(",")
    state.tinaType=AppConfig.Comma
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配浮点数
    */
  def matchFloat(): Unit ={
    matchSymbol("float")
    state.tinaType=AppConfig.T_Float
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配整点数
    */
  def matchInt(): Unit ={
    matchSymbol("int")
    state.tinaType=AppConfig.T_int
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配void关键字
    */
  def matchVoid(): Unit ={
    matchSymbol("void")
    state.tinaType=AppConfig.T_void
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配左花括号
    */
  def matchLeftBrace(): Unit ={
    matchSymbol("{")
    state.tinaType=AppConfig.Left_Brace
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配右花括号
    */
  def matchRightBrace(): Unit ={
    matchSymbol("}")
    state.tinaType=AppConfig.Right_Brace
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配等于号
    */
  def matchEqual(): Unit ={
    matchSymbol("=")
    state.tinaType=AppConfig.T_Equal
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配冒号
    */
  def matchSemicolon(): Unit ={
    matchSymbol(";")
    state.tinaType=AppConfig.T_Semicolon
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配return
    */
  def matchReturn(): Unit ={
    matchSymbol("return")
    state.tinaType=AppConfig.T_Return
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配Add
    */
  def matchAdd(): Unit ={
    matchSymbol("+")
    state.tinaType=AppConfig.T_Add
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  /**
    * 匹配变量
    */
  def matchId(): Unit ={
    state.tinaType=AppConfig.ID
    state.channel=TinaToken.DEFAULT_TOKEN_CHANNEL
  }

  def matchLetter(): Unit ={

  }

  def matchSymbol(s:String): Unit ={

  }
}
