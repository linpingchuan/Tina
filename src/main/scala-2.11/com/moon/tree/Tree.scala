package com.moon.tree

/**
  * Created by Lin on 17/4/9.
  */
trait Tree {
  def getParent():Tree

  def getPayload():Any

  def getChild(i:Int):Tree

  def getChildCount():Int

  def toStringTree():String

  def addChild(t:Tree)

  def deleteChild(i:Int)

  def isNil():Boolean

  def setChild(i:Int,t:Tree)


}
