package com.moon.tree.impl

import java.util.Objects

import com.moon.tree.Tree

import scala.collection.mutable.ListBuffer

/**
  * Created by Lin on 17/4/9.
  */
class BaseTree extends Tree{
  var children:ListBuffer[Tree]= _
  override def getParent(): Tree = ???

  override def getPayload(): Any = ???

  override def getChild(i: Int): Tree = {
    if(Objects.isNull(children)||i>=children.size)
      null
    else
      return children(i)
  }

  override def getChildCount(): Int = {
    if(Objects.nonNull(children))
      children.size
    else
      0
  }

  override def toStringTree(): String = {
    if(Objects.isNull(children)||children.isEmpty)
      return this.toString

  }

  override def addChild(t: Tree): Unit = {
    if(Objects.nonNull(t)){
      if(t.isNil()){

      }
    }
  }

  override def deleteChild(i: Int): Tree = {
    if(Objects.isNull(children))
      null
    else{
      children.remove(i)
    }
  }

  override def isNil(): Boolean = false

  override def setChild(i: Int, t: Tree): Unit = {
    if(Objects.nonNull(t)){
      if(t.isNil())
        throw new IllegalArgumentException("Can't set single child to a list")
      if(Objects.isNull(children)){
        children=ListBuffer[Tree]()
      }
      children.insert(i,t)
      t.setParent(this)
      t.setChildIndex(i)
    }
  }

  override def setParent(t: Tree): Unit = ???

  override def setChildIndex(index: Int): Unit = ???
}
