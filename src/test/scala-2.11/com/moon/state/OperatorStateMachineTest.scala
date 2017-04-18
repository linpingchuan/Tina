package com.moon.state

import com.moon.state.impl.OperatorStateMachine
import org.junit.Test

/**
  * Created by Lin on 17/4/16.
  */
class OperatorStateMachineTest {
  @Test
  def testStateMachine(): Unit ={
    val state=OperatorStateMachine.state(">")
    println(state)
  }


}
