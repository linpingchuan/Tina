package tina

/**
  * Created by Lin on 17/4/28.
  */
case class TinaToken(name: Any, kind: Int, line: Int, pos: Int) {
  override def toString: String = "name: "+name+" kind: "+TinaToken.tokenNames(kind)+" in line: "+line+" pos: "+pos
}

case class MisMatchTokenException(e: String) extends RuntimeException(e)

object TinaToken {
  val INT = 0
  val FLOAT = 1
  val LEFT_PARENT = 2
  val RIGHT_PARENT = 3
  val LEFT_BRACE = 4
  val RIGHT_BRACE = 5
  val TINA = 6
  val LOVE = 7
  val STRING = 8
  val ADD = 9
  val SUB = 10
  val MUL = 11
  val DIV = 12
  val WHILE = 13
  val UNIT = 14
  val RETURN = 15
  val FOR = 16
  val IF = 17
  val ELSE = 18
  val ELIF = 19
  val DOUBLE_QUOTATION = 20
  val SINGLE_QUOTATION = 21
  val CLASS = 22
  val COLON = 23
  val VAR=24
  val tokenNames = List[String](
    "int", "float", "(", ")", "{",
    "}", "tina", "love", "string", "+",
    "-", "*", "/", "while", "unit",
    "return", "for", "if", "else", "elif",
    "\"", "'", "class", ":","var"
  )
}

case class TinaLexer(src: Array[Char]) {
  var index: Int = 0
  var inferIndexs: List[Int] = List[Int]()

  var isInfer: Boolean = false
  var isComment: Boolean = false

  var buffer: List[TinaToken] = List[TinaToken]()
  var line: Int = 1

  def matchToken(kind: Int): TinaToken = {
    null
  }

  def skip(): Unit = index match {
    case lt if index < src.size => {
      val char = src(index)
      char match {
        case isSkip if (char == '\t' || char == ' ' || char == '\r') && (!isComment) => {
          index = index + 1
          skip()
        }
        case isEscaped if char == '\\' => {
          if (index + 2 < src.size)
            index = index + 2
          skip()
        }
        case isBeginComment if char == '/' && (index + 1) < src.size && src(index + 1) == '*' => {
          index = index + 2
          isComment = true
          skip()
        }
        case isEndComment if isComment && char == '*' && (index + 1) < src.size && src(index + 1) == '/' => {
          index = index + 2
          isComment=false
          skip()
        }
        case isInComment if isComment =>{
          index=index+1
          skip()
        }
        case _ => {}
      }
    }
    case _ => {}

  }

  def isNumber(): Boolean = {
    if (index < src.size) {
      val char = src(index)
      char match {
        case isNum if char >= '0' && char <= '9' => true
        case _ => false
      }
    }
    false
  }

  def isLetter():Boolean={
    var result=false
    if(index<src.size){
      val char=src(index)
      char match{
        case isTrue if (char>='a'&&char<='z')||(char>='A'&&char<='Z')||char=='_'||(char>='0'&&char<='9') =>result=true
        case _ => result=false
      }
    }
    result
  }

  def nextLetters(token:TinaToken):TinaToken=isLetter()match{
    case true => {
      val name=token.name.asInstanceOf[String]+src(index)
      if(name==TinaToken.tokenNames(TinaToken.IF)){
        index=index+1
        nextLetters(TinaToken(name,TinaToken.IF,line,token.pos+1))
      }else{
        index=index+1
        nextLetters(TinaToken(name,TinaToken.VAR,line,token.pos+1))
      }
    }
    case false => token
  }

  def nextNumber(token: TinaToken): TinaToken = {
    if (src(index) == '.' && token.kind == TinaToken.INT) {
      index = index + 1
      nextNumber(TinaToken(token.name.asInstanceOf[Float], TinaToken.FLOAT, token.line, token.pos + 1))
    }
    else if (src(index) == '.' && token.kind == TinaToken.FLOAT)
      throw MisMatchTokenException("excepting original type is int but found float.Is double dot in this number :) ")
    else if (isNumber()) {
      val int = src(index).toInt - '0'.toInt
      index = index + 1
      nextNumber(TinaToken(token.name.asInstanceOf[Int] * 10 + int, token.kind, token.line, token.pos + 1))
    } else
      token
  }

  def nextToken(): TinaToken = {
    var token:TinaToken=null
    if (isInfer)
      inferIndexs = index +: inferIndexs

    if (index < src.size) {
      val char = src(index)
      char match {
        case skipToken if char == '\t' || char == ' ' || char == '\r' || char == '/' => {
          skip()
          val flag=isInfer
          isInfer=false
          token=nextToken()
          isInfer=flag
        }
        case nextLine if char == '\n' => {
          inferIndexs = index +: inferIndexs
          index = 0
          line = line + 1
          val flag=isInfer
          isInfer=false
          token=nextToken()
          isInfer=flag
        }
        case isNum if isNumber() => {
          token=nextNumber(TinaToken('0'.toInt, TinaToken.INT, line, index-1))
        }
        case isLetters if isLetter() =>{
          token=nextLetters(TinaToken("",TinaToken.VAR,line,index))
        }
      }
    }


    if (isInfer) {
      index = inferIndexs.head
      inferIndexs=inferIndexs.slice(1, inferIndexs.size)
    }

    token
  }

  /**
    * 向前同步N个token，有利于判断该Token是哪个Token
    *
    * @param size
    */
  def syn(size: Int): Unit = {

  }
}