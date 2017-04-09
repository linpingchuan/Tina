package com.moon.tree.impl

import com.moon.tree.Tree

/**
  * Created by Lin on 17/4/9.
  */
class BaseTree extends Tree{
  override def getParent(): Tree = ???

  override def getPayload(): Any = ???

  override def getChild(i: Int): Tree = ???

  override def getChildCount(): Int = ???

  override def toStringTree(): String = ???

  override def addChild(t: Tree): Unit = ???

  override def deleteChild(i: Int): Unit = ???

  override def isNil(): Boolean = ???

  override def setChild(i: Int, t: Tree): Unit = ???
}
