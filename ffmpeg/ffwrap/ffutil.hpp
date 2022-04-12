#pragma once

#ifndef FF_SET_CODE_S
#define FF_SET_CODE_S(v, s) this->code = { v, s, __LINE__ }
#endif

#ifndef FF_SET_CODE
#define FF_SET_CODE(v) this->code = { v, "", __LINE__ }
#endif

#ifndef FF_GET_CODE
#define FF_GET_CODE() std::get<0>(this->code)
#endif
