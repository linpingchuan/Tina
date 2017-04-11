package com.moon.tree.impl

import java.util.Objects

import com.moon.token.TinaToken
import com.moon.tree.Tree

import scala.collection.mutable.ListBuffer

/**
  * Created by Lin on 17/4/9.
  */
class BaseTree extends Tree {
  var parent: BaseTree = _

  var children: ListBuffer[Tree] = _

  var token: TinaToken = _

  var childIndex: Int = -1
  var startIndex: Int = -1
  var stopIndex: Int = -1

  override def getParent(): Tree = null

  override def getPayload(): Any = ???

  override def getChild(i: Int): Tree = {
    if (Objects.isNull(children) || i >= children.size)
      null
    else
      return children(i)
  }

  override def getChildCount(): Int = {
    if (Objects.nonNull(children))
      children.size
    else
      0
  }

  override def toStringTree(): String = {
    if (Objects.isNull(children) || children.isEmpty)
      return this.toString
    val buf = new StringBuilder
    if (!isNil()) {
      buf.append("(")
      buf.append(this.toString)
      buf.append(' ')
    }
    for (i <- 0 until children.size) {
      val t = children(i)
      if (i > 0)
        buf.append(' ')
      buf.append(t.toStringTree())
    }
    if (!isNil())
      buf.append(')')
    buf.toString
  }

  override def addChild(t: Tree): Unit = {
    if (Objects.nonNull(t)) {
      var childTree: BaseTree = t.asInstanceOf[BaseTree]

      childTree match {
        case nil if childTree.isNil() => {
          if (Objects.nonNull(this.children) && this.children == childTree.children)
            throw new RuntimeException("attempt to add child list to itself")
        }
        case nonNull if Objects.nonNull(childTree.children) => {
          this.children match {
            case childrenNonNull if Objects.nonNull(this.children) => {
              val n = childTree.children.size
              for (i <- 0 until n) {
                val c = childTree.children(i)
                this.children += c
                c.setParent(this)
                c.setChildIndex(children.size - 1)
              }
            }
            case _ => {
              this.children = childTree.children
              this.freshParentAndChildIndexes()
            }
          }
        }
        case _ => {
          if (Objects.isNull(children))
            children = new ListBuffer[Tree]

          children += t
          childTree.setParent(this)
          childTree.setChildIndex(children.size - 1)
        }
      }

    }
  }

  override def deleteChild(i: Int): Tree = {
    if (Objects.isNull(children))
      null
    else {
      children.remove(i)
    }
  }

  override def isNil(): Boolean = false

  override def setChild(i: Int, t: Tree): Unit = {
    if (Objects.nonNull(t)) {
      if (t.isNil())
        throw new IllegalArgumentException("Can't set single child to a list")
      if (Objects.isNull(children)) {
        children = ListBuffer[Tree]()
      }
      children.insert(i, t)
      t.setParent(this)
      t.setChildIndex(i)
    }
  }

  override def setParent(t: Tree): Unit = {}

  override def setChildIndex(index: Int): Unit = ???

  override def getLine(): Int = ???

  override def getCharPositionInLine(): Int = ???

  override def getAncestors(): ListBuffer[Tree] = getParent() match {
    case null => null
    case _ => {
      val ancestors = ListBuffer[Tree]()
      var t: Tree = this
      t = t.getParent()
      while (Objects.nonNull(t)) {
        ancestors.insert(0, t)
        t = t.getParent()
      }
      ancestors
    }
  }

  override def getAncestor(ttype: Int): Tree = {
    var t: Tree = this.asInstanceOf[Tree]
    t = t.getParent()
    while (Objects.nonNull(t)) {
      if (t.getType() == ttype)
        return t
      t = t.getParent()
    }
    null
  }


  override def getType(): Int = ???

  override def hasAncestor(ttype: Int): Boolean = Objects.nonNull(getAncestor(ttype))

  override def freshParentAndChildIndexes(): Unit = freshParentAndChildIndexes(0)

  override def freshParentAndChildIndexes(offset: Int): Unit = {
    val n: Int = getChildCount()
    for (c <- offset until n) {
      val child = getChild(c)
      child.setChildIndex(c)
      child.setParent(this)
    }
  }
}
