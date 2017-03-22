package com.moon.lexer.impl

import com.moon.config
import com.moon.config.AppConfig
import com.moon.lexer.Lexer
import com.moon.token.{TinaTokenException, TinaToken}

/**
  * Created by lin on 3/14/17.
  */
case class TinaLexer(src: String) extends Lexer {
  var ch: Char = _
  var index: Int = _
  consume()

  def isLetter(): Boolean = {
    ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z'
  }

  def nextToken(): TinaToken = {
    while (ch != AppConfig.EOF.toChar) {
      ch match {
        case space if ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' => consume()
        case ',' => {
          consume()
          return new TinaToken(",", AppConfig.COMMA)
        }
        case '+' => {
          consume()
          return new TinaToken("+", AppConfig.ADD)
        }
        case '=' => {
          consume()
          return new TinaToken("=",AppConfig.EQUALS)
        }
        case letter if isLetter() =>{
          return letters()
        }
        case num if isNumber() => {
          return number()
        }
        case _ => throw new TinaTokenException("could not found any legal token.")
      }
    }
    new TinaToken("<EOF>", AppConfig.EOF)
  }

  def letters():TinaToken={
    def append(buf:StringBuilder):StringBuilder=ch match{
      case letter if isLetter() => {
        buf.append(ch)
        consume()
        append(buf)
      }
      case _ => buf
    }
    new TinaToken(append(new StringBuilder).toString(),AppConfig.NAME)
  }


  def isNumber():Boolean=ch match{
    case num if ch >= '0' && ch <= '9' => true
    case _ => false
  }

  def number(): TinaToken = {
    def isNum(): Boolean =
      ch match {
        case num if ch >= '0' && ch <= '9' => true
        case space if ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == AppConfig.EOF.toChar => false
        case _ => throw new RuntimeException("expected number or space but found " + ch)
      }
    def add(result:Int): Int = ch match {
      case num if isNum() => {
        val sum =  result* 10 + (ch.toInt - '0'.toInt)
        consume()
        add(sum)
      }
      case _ => result
    }

    new TinaToken(add(0), AppConfig.INTEGER)
  }

  def consume(): Unit = {

    index match {
      case gtEq if index >= src.length => ch = AppConfig.EOF.toChar
      case _ => ch = src.charAt(index)
    }
    index += 1
  }

  def matchToken(x: Char): Unit = {
    x match {
      case eq if x == ch => consume()
      case _ => throw new RuntimeException("expected " + x + "; found " + ch)
    }
  }
}
