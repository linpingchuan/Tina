package tina

import org.junit.Test


/**
  * Created by Lin on 17/4/29.
  */
class TinaTokenTest {
  @Test
  def testSkip(): Unit ={
    val lexer=new TinaLexer("    /* i love my family */ a".toCharArray);
    val token=lexer.nextToken()
    println(token)
  }
}
