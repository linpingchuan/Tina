package com.moon.lexer

import com.moon.lexer.impl.TinaLexer
import com.moon.parser.impl.TinaParser
import com.moon.symbol.impl.SymbolTable
import org.junit.Test

/**
  * Created by lin on 3/25/17.
  */
class TinaParserTest {
  @Test
  def testVarDeclarations(): Unit ={
    val lexer=new TinaLexer("love a,,")
    val parser=new TinaParser(lexer,new SymbolTable)
    println(parser.speculateVarDeclaration())
    println(parser.index)
  }

  @Test
  def testRemoveIndex(): Unit ={
    val list=List("1","3")
    val (start,_::end)=list.splitAt(1)
    println(start:::end)
  }
}
