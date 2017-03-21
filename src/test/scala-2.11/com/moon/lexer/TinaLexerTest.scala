package com.moon.lexer

import com.moon.lexer.impl.TinaLexer
import org.junit.Test

/**
  * Created by lin on 3/21/17.
  */
class TinaLexerTest {
  /**
    * 整数转换测试
    */
  @Test
  def testNumber(): Unit ={
    val lexer=new TinaLexer("788")
    val num=lexer.nextToken()
    println(num)
  }

  /**
    * 变量检测
    */
  @Test
  def testLetter(): Unit ={
    val lexer=new TinaLexer("trrr")
    val letter=lexer.nextToken()
    println(letter)
  }
}
