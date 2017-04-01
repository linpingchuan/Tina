package com.moon.parser.impl

import java.lang.Exception

import com.moon.config.AppConfig
import com.moon.lexer.impl.TinaLexer
import com.moon.symbol.impl.{TinaLove, VariableSymbol, SymbolTable}
import com.moon.token.TinaToken

import scala.annotation.tailrec

/**
  * Created by lin on 3/14/17.
  */
case class TinaParser(lexer: TinaLexer, symtab: SymbolTable) {
  // 栈，存放用于记录位置的位标
  var markers = List[Int]()
  // 大小可变的缓冲区
  var lookahead = List[TinaToken]()
  // 当前向前看词法单元的下标
  var index: Int = 0
  // 是否在进行演绎
  var isNotSpeculated: Boolean = _
  sync(1)

  def compilationUnit(): Unit = {

  }

  def speculateVarDeclaration(): Boolean = {
    var success = true
    isNotSpeculated = false
    mark()

    success = varDeclaration()

    release()
    success
  }

  /**
    * 变量声明应该符合以下格式:
    * love a 或者 love a,b
    *
    * @return
    */
  def varDeclaration(): Boolean = {
    def matchVarDeclarations(): Boolean = try {
      matchToken(AppConfig.COMMA)
      try {
        defVarToken(matchToken(AppConfig.NAME))
      } catch {
        case e: MismatchedTokenException => {
          throw new MismatchedVarNameException(e.getMessage)

        }
      }

      true
    } catch {
      case e: MismatchedTokenException => {
        false
      }
    }
    @tailrec
    def recursiveMatch(a: () => Boolean): Boolean = a() match {
      case true => recursiveMatch(a)
      case _ => false
    }

    matchToken(AppConfig.LOVE)

    defVarToken(matchToken(AppConfig.NAME))
    recursiveMatch(matchVarDeclarations)
  }

  def defVarToken(token: TinaToken): Unit = {
    if (isNotSpeculated){
      val vs = new VariableSymbol(token.input.toString, new TinaLove)
      symtab.define(vs)
    }
  }

  /**
    * 多变量定义格式
    * love [a,b,c]=[e,f,g]
    *
    * [a,b,c]=[e,f,g]
    */
  def varsAssignment():Unit={
    def matchComma():Boolean={
      try{
        defVarToken(matchToken(AppConfig.COMMA))
        true
      }catch{
        case e:MismatchedTokenException => false
      }
    }

    def matchTokenComma(f:()=>Boolean):Unit=f() match{
      case true => {
        defVarToken(matchToken(AppConfig.NAME))
        matchTokenComma(f)
      }
      case false => Unit
    }

    /**
      * [a,b]
      */
    def vars():Unit={
      defVarToken(matchToken(AppConfig.LEFT_BRACKET))
      defVarToken(matchToken(AppConfig.NAME))
      matchTokenComma(matchComma)
      defVarToken(matchToken(AppConfig.RIGHT_BRACKET))
    }

    defVarToken(matchToken(AppConfig.LOVE))
    vars()
    defVarToken(matchToken(AppConfig.EQUALS))
    vars()
  }

  /**
    * 变量定义格式
    * love a=b
    *
    * a=b
    *
    */
  def varAssignment(): Unit = {
    def assignment():Unit={
      defVarToken(matchToken(AppConfig.NAME))
      defVarToken(matchToken(AppConfig.EQUALS))
      defVarToken(matchToken(AppConfig.NAME))
    }

    def recursiveAssignment():Boolean={
      var flag:Boolean=false
      try{
        defVarToken(matchToken(AppConfig.COMMA))
        flag=true
      }catch{
        case e:MismatchedTokenException => flag=false
      }
      if(flag){
        assignment()
      }
      flag
    }
    @tailrec
    def recursiveMatch(f:()=>Boolean): Unit =f() match{
      case true => recursiveMatch(f)
      case false => Unit
    }

    matchToken(AppConfig.LOVE)
    assignment()
    recursiveMatch(recursiveAssignment)
  }



  def expression(): Unit = {

  }


  def consume(): Unit = {
    index += 1

    /**
      * 非推断状态，而且到达向前看缓冲区的末尾
      * 到了末尾，就该重新从0开始填入新的词法单元
      * 大小清0，回收内存
      */
    if (index == lookahead.size && markers.size == 0) {
      index = 0
      lookahead = List[TinaToken]()
    }

    sync(1)
  }

  /**
    * 确保当前位置p之前有i个词法单元
    * 判断是否越界，如果越界获取n个词法单元
    *
    * @param i
    */
  def sync(i: Int): Unit = {
    def fill(n: Int): Unit = {
      for (f <- 1 to n) {
        lookahead = lookahead :+ lexer.nextToken()
      }

    }

    if (i + index > lookahead.size) {
      val n = i + index - lookahead.size
      fill(n)

    }
  }

  /**
    * 标记位标
    *
    * @return
    */
  def mark(): Int = {
    markers = markers :+ index
    index
  }

  /**
    * 释放位标
    */
  def release(): Unit = {
    val marker = markers(markers.size - 1)
    val (start, _ :: end) = markers.splitAt(markers.size - 1)
    markers = start ::: end
    index = marker
  }

  def lookaheadToken(i: Int): TinaToken = {
    sync(i)
    lookahead(i + index - 1)
  }

  def matchToken(x: Int): TinaToken = x match {
    case eqType if lookaheadToken(1).tinaType == x => {
      val token = lookaheadToken(1)
      consume()
      token
    }
    case _ => throw new MismatchedTokenException("expecting " + AppConfig.tokenNames(x) + " found " + lookaheadToken(1).input)
  }
}

case class MismatchedTokenException(e: String) extends RuntimeException(e)

class MismatchedVarNameException(es: String) extends MismatchedTokenException(es)
