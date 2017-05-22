package tina

import org.junit.Test

/**
  * Created by Lin on 17/5/22.
  */
class ExpressionStateMachineTest {
  @Test
  def testBinaryExp(): Unit ={
    val lexer=TinaLexer("23*b".toCharArray)
    val state=ExpressionStateMachine(lexer)

    println(state.binaryExp(lexer).asInstanceOf[BinaryExpression])
  }
}

