package com.moon.lexer

import com.moon.config.AppConfig
import com.moon.lexer.impl.TinaLexer
import com.moon.token.TinaToken
import org.junit.Test

/**
  * Created by lin on 3/21/17.
  */
class TinaLexerTest {
  /**
    * 整数转换测试
    */
  @Test
  def testNumber(): Unit = {
    val lexer = new TinaLexer("788")
    val num = lexer.nextToken()
    println(num)
  }

  /**
    * 变量检测
    */
  @Test
  def testLetter(): Unit = {
    val lexer = new TinaLexer("trrr")
    val letter = lexer.nextToken()
    println(letter)
  }

  /**
    * 非法字符测试
    */
  @Test
  def testEOF(): Unit = {
    val lexer = new TinaLexer("-=")
    val eof = lexer.nextToken()
    println(eof)
  }

  @Test
  def testAll(): Unit = {
    val lexer = new TinaLexer(" t=23 p=23 love\n" +
      "=t+p")
    def printToken(token: TinaToken): Unit = token match {
      case notEof if token.tinaType != AppConfig.EOF => {
        println(token)
        printToken(lexer.nextToken())
      }
      case _ => println("I love Tina")
    }
    printToken(lexer.nextToken())

  }
}
