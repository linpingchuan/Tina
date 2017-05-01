package tina

import org.junit.Test


/**
  * Created by Lin on 17/4/29.
  */
class TinaTokenTest {
  @Test
  def testSkip(): Unit ={
    val lexer=new TinaLexer(("love tina(){\n" +
      "println(c)}").toCharArray);
    def printToken(lexer:TinaLexer): Unit ={
      val token=lexer.nextToken()
      token match{
        case null => {}
        case _ => {
          println(token)
          printToken(lexer)
        }
      }
    }

   lexer.syn(20)
    lexer.buffer.foreach( x => println(x))
    println(lexer.inferIndexs)
    printToken(lexer)
  }
}
