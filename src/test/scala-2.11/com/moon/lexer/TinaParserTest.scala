package com.moon.lexer

import com.moon.lexer.impl.TinaLexer
import com.moon.parser.impl.TinaParser
import com.moon.symbol.impl.SymbolTable
import org.junit.Test

import scala.collection.mutable.ListBuffer

/**
  * Created by lin on 3/25/17.
  */
class TinaParserTest {
  @Test
  def testVarDeclarations(): Unit ={
    val lexer=new TinaLexer("love a,b,c,d,f")
    val parser=new TinaParser(lexer,new SymbolTable)
//    println(parser.speculateVarDeclaration())
    parser.isNotSpeculated=true
    println(parser.varDeclaration())
    println(parser.index)

    println(parser.symtab.symbols)
  }

  @Test
  def testRemoveIndex(): Unit ={
    val list=List("1","3")
    val (start,_::end)=list.splitAt(1)
    println(start:::end)
  }

  @Test
  def testVarAssignment(): Unit ={
    val lexer=new TinaLexer("love a=b,c=d")
    val parser=new TinaParser(lexer,new SymbolTable)
    parser.isNotSpeculated=true
    println(parser.varAssignment())
    println(parser.index)
    println(parser.symtab.symbols)
  }

  @Test
  def testVarsAssignment(): Unit ={
    val lexer=new TinaLexer("love [a12332,b,c,d]=[e,f,g,h]")
    val parser=new TinaParser(lexer,new SymbolTable)
    parser.isNotSpeculated=true
    println(parser.varsAssignment())
    println(parser.index)
    println(parser.symtab.symbols)
  }

  @Test
  def testTo1(): Unit ={
    for (i <- 1 to 1){
      println(i)
    }
  }

  @Test
  def testListBuffer(): Unit ={
    var list=ListBuffer[String]("1","2","3","4","5")
    list+= "6"
    println(list)
  }
}
