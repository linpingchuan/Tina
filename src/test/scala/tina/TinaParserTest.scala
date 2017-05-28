package tina

import org.junit.Test

/**
  * Created by Lin on 17/5/13.
  */
class TinaParserTest {
  @Test
  def testRun(): Unit ={
    val lexer=TinaLexer((" love test(a, b){ " +
      "c=a+(b+23)*b; " +
      " run(a,2,3,4) " +
//      " if(a>1){ run(1) } " +
//      " return c+23+12 " +
      "}").toCharArray)
    val compiler=TinaParser(lexer)
    compiler.run()
//    println(compiler.globalVariable)
//    println(new GsonBuilder().setPrettyPrinting().create().toJson(compiler.functionDeclarations(0)))
    println(compiler.functionDeclarations(0))
    compiler.functionDeclarations.head.body.body.foreach(x=>println(x))
  }
}

