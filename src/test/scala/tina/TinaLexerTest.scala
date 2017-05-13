package tina

import org.junit.Test


/**
  * Created by Lin on 17/4/29.
  */
class TinaLexerTest {
  @Test
  def testSkip(): Unit ={
    val lexer=new TinaLexer(("love tina(){\n" +
      "println(c) = }").toCharArray);
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

class TinaStateMachineTest{
  @Test
  def testNextTinaState(): Unit ={
    val lexer=TinaLexer(">>= ==".toCharArray)
    val tinaStateMachine=TinaStateMachine(lexer)
    val state=tinaStateMachine.nextTinaState()
    println(state)
    val state1=tinaStateMachine.nextTinaState()
    println(state1)
    lexer.index=lexer.index+1
    val state2=tinaStateMachine.nextTinaState()
    println(state2)
  }
}