package com.moon.lexer

import com.moon.lexer.impl.TinaLexer
import org.junit.Test

/**
  * Created by lin on 3/21/17.
  */
class TinaLexerTest {
  @Test
  def testNumber(): Unit ={
    val lexer=new TinaLexer("788")
    val num=lexer.nextToken()
    println(num)
  }

}
