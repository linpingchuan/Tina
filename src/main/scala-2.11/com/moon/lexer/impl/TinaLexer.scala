package com.moon.lexer.impl

import com.moon.config
import com.moon.config.AppConfig
import com.moon.lexer.Lexer
import com.moon.token.TinaToken

/**
  * Created by lin on 3/14/17.
  */
case class TinaLexer(src: String) extends Lexer {
  var ch: Char = _
  var index:Int = _
  def isLetter(): Boolean = {
    ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z'
  }

  def nextToken(): TinaToken = {
    while (ch != AppConfig.EOF.toChar) {
      ch match {
        case space if ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' => consume()
        case ','=> {
          consume()
          return new TinaToken(",",AppConfig.COMMA)
        }
        case '+'=>{
          consume()
          return new TinaToken("+",AppConfig.ADD)
        }
        case number if ch>='0'&&ch<='9'=>{

        }
      }
    }
    new TinaToken("<EOF>", AppConfig.EOF)
  }

  def number():TinaToken={
    null
  }

  def consume(): Unit ={
    index+=1
    index match{
      case gtEq if index >=src.length => ch=AppConfig.EOF.toChar
      case _ => src.charAt(index)
    }
  }

  def matchToken(x:Char): Unit ={
    x match{
      case eq if x==ch => consume()
      case _ => throw new RuntimeException("expected "+x+"; found "+ch)
    }
  }
}
