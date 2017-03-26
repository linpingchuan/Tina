package com.moon.symbol.impl

import com.moon.symbol.TinaSymbol
import com.moon.tp.TinaType

/**
  * Created by lin on 3/14/17.
  */
class VariableSymbol(nm:String, tp:TinaType) extends TinaSymbol(nm,tp) {
  override def toString: String = "[tp.getName() => "+nm+" ]"
}

class TinaLove extends TinaType{
  override def getName(): String = "love"
}
