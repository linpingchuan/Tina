package tina

import org.junit.Test

/**
  * Created by Lin on 17/5/13.
  */
class TinaParserTest {
  @Test
  def testRun(): Unit ={
    val lexer=TinaLexer((" love test(a, b){" +
      "c=a+1+b+23+d" +
      "}").toCharArray)
    val compiler=TinaParser(lexer)
    compiler.run()
    println(compiler.globalVariable)
    println(compiler.functionDeclarations)
    println(compiler.functionDeclarations(0).body)
  }
}
