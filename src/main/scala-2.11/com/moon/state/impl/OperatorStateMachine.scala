package com.moon.state.impl

import com.moon.state.TinaState

/**
  * Created by Lin on 17/4/12.
  */
object OperatorStateMachine {
  def matchState(operator:String,opIndex:Int,group:Int,subStateIndex:Int,subStateCount:Int,state:OperatorState):OperatorState={
    val operatorStates=group match{
      case 0 => OperatorStateMachineConfig.state0
      case 1 => OperatorStateMachineConfig.state1
      case 2 => OperatorStateMachineConfig.state2
    }

    for(index <- subStateIndex to subStateIndex+subStateCount if opIndex<operator.toCharArray.size){
      operatorStates(index) match{
        case eqState if operatorStates(index).char==operator.charAt(opIndex)=>{
          operatorStates(index).subCount match{
            case 0 => return operatorStates(index)
            case _ => return matchState(operator,opIndex+1,group+1,operatorStates(index).subStateIndex,operatorStates(index).subCount,operatorStates(index))
          }

        }
        case _ =>{}
      }

    }

    state
  }

  def state(operator:String):OperatorState={
    matchState(operator,0,0,0,OperatorStateMachineConfig.state0.size,null)
  }
}

object OperatorStateMachineConfig{
  val state0:List[OperatorState]=List(
    OperatorState('+',0,2,0,"+"),
    OperatorState('-',2,2,0,"-"),
    OperatorState('>',4,2,0,">")
  )

  val state1:List[OperatorState]=List(
    OperatorState('+',0,0,0,"++"),
    OperatorState('=',0,0,0,"+="),
    OperatorState('-',0,0,0,"--"),
    OperatorState('=',0,0,0,"-="),
    OperatorState('=',0,0,0,">="),
    OperatorState('>',0,1,0,">>")
  )

  val state2:List[OperatorState]=List(
    OperatorState('=',0,0,0,">>=")
  )
}

case class OperatorState(char:Char,subStateIndex:Int,subCount:Int,index:Int,desc:String) extends TinaState{

}
