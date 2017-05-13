package tina

import org.junit.Test

/**
  * Created by Lin on 17/5/13.
  */
class TinaParserTest {
  @Test
  def testRun(): Unit ={
    val lexer=TinaLexer(" love test(){}".toCharArray)
    val compiler=TinaParser(lexer)
    compiler.run()
    println(compiler.globalVariable)
  }
}