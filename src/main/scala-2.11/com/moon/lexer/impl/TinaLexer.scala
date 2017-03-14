package com.moon.lexer.impl

import com.moon.lexer.Lexer
import com.moon.state.impl.RecognizerSharedState

/**
  * Created by lin on 3/14/17.
  */
case class TinaLexer(src:String,state:RecognizerSharedState) extends Lexer{
  def this(str:String){
    this(str,new RecognizerSharedState)
  }

  /**
    * 从左括号开始
    */
  def startBracket(): Unit ={

  }

  def matchSymbol(s:String): Unit ={

  }
}
