package com.moon.state.impl

import com.moon.state.TinaState
import com.moon.token.TinaToken

/**
  * Created by lin on 3/14/17.
  */
class RecognizerSharedState extends TinaState {
  var token: TinaToken = _
  var channel: Int = _
  var tinaType: Int = _
}
